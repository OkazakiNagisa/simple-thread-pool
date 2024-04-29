#pragma once
#include <atomic>
#include <deque>
#include <functional>
#include <future>
#include <memory>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

namespace stp
{

template <typename F, typename... Args>
concept CallableWithArgs = requires(F f, Args... args) { f(args...); };

class ThreadPool
{
public:
    explicit ThreadPool(
        const unsigned int threadsCount = std::thread::hardware_concurrency())
        : SuicideFlag(false)
    {
        for (int i = 0; i < threadsCount; i++)
        {
            Threads.emplace_back([this, i]() {
                while (true)
                {
                    std::function<void()> task;
                    {
                        auto lock =
                            std::unique_lock<std::mutex>(this->TasksMutex);
                        this->ConditionVar.wait(lock, [this]() {
                            return !this->Tasks.empty() ||
                                   SuicideFlag.load(std::memory_order::relaxed);
                        });

                        if (this->SuicideFlag)
                            return;
                        else
                        {
                            task = std::move(this->Tasks.back());
                            Tasks.pop_back();
                        }
                    }
                    task();
                }
            });
        }
    }

    ~ThreadPool()
    {
        SuicideFlag.store(true, std::memory_order::relaxed);
        ConditionVar.notify_all();
        for (auto &&thread : Threads)
        {
            thread.join();
        }
    }

    ThreadPool(ThreadPool &&) = delete;
    ThreadPool &operator==(ThreadPool &&) = delete;
    ThreadPool(const ThreadPool &) = delete;
    ThreadPool &operator==(const ThreadPool &) = delete;

    template <typename F, typename... Args>
        requires CallableWithArgs<F, Args...>
    auto EnqueueTask(F &&task, Args &&...parameters)
        -> std::future<std::invoke_result_t<F, Args...>>
    {
        using Ret = std::invoke_result_t<F, Args...>;
        auto boundTask = [task = std::forward<F>(task),
                          ... parameters = std::forward<Args>(parameters)] {
            return task(parameters...);
        };
        auto packaged = std::make_shared<std::packaged_task<Ret()>>(boundTask);
        auto ret = packaged->get_future();
        {
            auto lock = std::lock_guard<std::mutex>(TasksMutex);
            Tasks.emplace_front(
                [packaged = std::move(packaged)] { (*packaged)(); });
        }
        ConditionVar.notify_one();
        return ret;
    }

    void WaitAll()
    {
        auto lock = std::unique_lock<std::mutex>(this->TasksMutex);
        this->ConditionVar.wait(lock, [this]() { return this->Tasks.empty(); });
    }

private:
    std::atomic_bool SuicideFlag;
    std::vector<std::thread> Threads;
    std::deque<std::function<void()>> Tasks;
    std::mutex TasksMutex;
    std::condition_variable ConditionVar;
};

} // namespace stp
#pragma once
#include <atomic>
#include <deque>
#include <functional>
#include <future>
#include <memory>
#include <thread>
#include <utility>
#include <vector>

namespace stp
{

class ThreadPool
{
public:
    explicit ThreadPool(const int threadsCount) : SuicideFlag(false)
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
                            return this->Tasks.size() > 0 ||
                                   SuicideFlag.load(std::memory_order::acquire);
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
        SuicideFlag.store(true, std::memory_order::release);
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

    // std::future<int> EnqueueTask(std::function<int(int)> task, int parameter)
    // {
    //     auto packaged = std::packaged_task<int()>(std::bind(task,
    //     parameter)); auto ret = packaged.get_future();
    //     {
    //         auto lock = std::lock_guard<std::mutex>(TasksMutex);
    //         Tasks.push_front(std::move(packaged));
    //     }
    //     ConditionVar.notify_one();
    //     return ret;
    // }

    // template <typename Ret, typename Args>
    // std::future<Ret> EnqueueTask(std::function<Ret(Args)> task,
    //                              Args parameters)
    // {
    //     auto packaged =
    //         std::packaged_task<Ret(Args)>(std::bind(task, parameters));
    //     auto ret = packaged.get_future();
    //     {
    //         auto lock = std::lock_guard<std::mutex>(TasksMutex);
    //         Tasks.emplace_front([=] { packaged(); });
    //     }
    //     ConditionVar.notify_one();
    //     return ret;
    // }

    // template <typename F, typename... Args>
    // auto EnqueueTask(F &&task, Args &&...parameters)
    //     -> std::future<decltype(task(parameters...))>
    // {
    //     using Ret = decltype(task(parameters...));
    //     auto packaged =
    //         std::packaged_task<Ret()>(std::bind(std::forward<F>(task),
    //         std::forward<Args>(parameters)...));
    //     auto ret = packaged.get_future();
    //     auto p = packaged; // packaged_task not copy-constructable
    //     std::function<void()> f = [=] -> void { packaged(); };
    //     {
    //         auto lock = std::lock_guard<std::mutex>(TasksMutex);
    //         Tasks.emplace_front(f);
    //     }
    //     ConditionVar.notify_one();
    //     return ret;
    // }

    // template <typename F, typename... Args>
    // auto EnqueueTask(F &&task, Args &&...parameters)
    //     -> std::future<decltype(task(parameters...))>
    // {
    //     using Ret = decltype(task(parameters...));
    //     auto packaged =
    //         std::packaged_task<Ret()>(std::bind(std::forward<F>(task),
    //         std::forward<Args>(parameters)...));
    //     auto ret = packaged.get_future();
    //     std::function<void()> f = [p = std::move(packaged)] mutable -> void {
    //     /* p();
    //     */ };
    //     {
    //         auto lock = std::lock_guard<std::mutex>(TasksMutex);
    //         Tasks.emplace_front(f);
    //     }
    //     ConditionVar.notify_one();
    //     return ret;
    // }

    template <typename F, typename... Args>
    auto EnqueueTask(F &&task, Args &&...parameters)
        -> std::future<decltype(task(parameters...))>
    {
        using Ret = decltype(task(parameters...));
        auto packaged = std::make_shared<std::packaged_task<Ret()>>(std::bind(
            std::forward<F>(task), std::forward<Args>(parameters)...));
        auto ret = packaged->get_future();
        {
            auto lock = std::lock_guard<std::mutex>(TasksMutex);
            Tasks.emplace_front([=] -> void { (*packaged)(); });
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
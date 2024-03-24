#pragma once
#include <deque>
#include <future>
#include <thread>
#include <vector>

namespace stp
{

class ThreadPool
{
public:
    explicit ThreadPool(const int threadsCount)
        : SuicideFlag(false), Threads(threadsCount)
    {
        for (int i = 0; i < threadsCount; i++)
        {
            Threads.emplace_back([this, i]() {
                while (true)
                {
                    std::packaged_task<int()> task;
                    {
                        auto lock =
                            std::unique_lock<std::mutex>(this->TasksMutex);
                        this->ConditionVar.wait(lock, [this]() {
                            return this->Tasks.size() > 0 || this->SuicideFlag;
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
        {
            auto lock = std::lock_guard<std::mutex>(TasksMutex);
            SuicideFlag = true;
        }
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

    std::future<int> EnqueueTask(std::function<int(int)> task, int parameter)
    {
        auto packaged = std::packaged_task<int()>(std::bind(task, parameter));
        auto ret = packaged.get_future();
        {
            auto lock = std::lock_guard<std::mutex>(TasksMutex);
            Tasks.push_front(std::move(packaged));
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
    bool SuicideFlag;
    std::vector<std::thread> Threads;
    std::deque<std::packaged_task<int()>> Tasks;
    std::mutex TasksMutex;
    std::condition_variable ConditionVar;
};

} // namespace stp
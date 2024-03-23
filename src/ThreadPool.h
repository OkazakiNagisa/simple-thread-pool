#pragma once
#include <deque>
#include <future>
#include <thread>
#include <vector>

class ThreadPool
{
public:
    explicit ThreadPool(const int threads) : Threads(threads)
    {
        for (int i = 0; i < threads; i++)
        {
            Threads.emplace_back(std::thread([this, i]() {
                while (true)
                {
                    std::this_thread::yield();
                }
            }));
        }
    }

    // ThreadPool(ThreadPool &&) = default;
    ThreadPool(const ThreadPool &) = delete;
    ThreadPool &operator==(const ThreadPool &) = delete;

    std::function<void(void)> ConsumeTask()
    {
        auto lock = std::lock_guard<std::mutex>(TasksMutex);
        auto ret = Tasks.back();
        Tasks.pop_back();
        return ret;
    }

    void EnqueueTask(std::function<void(void)> task)
    {
        auto lock = std::lock_guard<std::mutex>(TasksMutex);
        Tasks.push_front(task);
    }

private:
    std::vector<std::thread> Threads;
    std::deque<std::function<void(void)>> Tasks;
    std::mutex TasksMutex;
};
#include <catch2/catch_test_macros.hpp>
#include <stp/ThreadPool.h>

using stp::ThreadPool;

TEST_CASE("Evaluate task execution correctness", "[stp]")
{
    ThreadPool tp;
    auto f = [](int a) {
        int ret = 0;
        for (int i = 0; i < a; i++)
        {
            ret += 10 * i;
        }
        return ret;
    };
    auto future = tp.EnqueueTask(f, 1000000);

    REQUIRE(future.get() == f(1000000));
    tp.WaitAll();
}

TEST_CASE("Multi tasks", "[stp]")
{
    ThreadPool tp;
    auto f = [](int in, int num) {
        int ret = 0;
        for (int i = 0; i < in; i++)
        {
            ret += 10 * i;
        }
        return std::tuple(ret, num);
    };
    std::vector<std::future<std::tuple<int, int>>> futures;
    for (int i = 0; i < 10000; i++)
    {
        futures.emplace_back(tp.EnqueueTask(f, 1000000, i));
    }

    auto [result, _] = f(1000000, 0);
    for (auto &&future : futures)
    {
        auto [ret, num] = future.get();
        REQUIRE(ret == result);
    }

    tp.WaitAll();
}
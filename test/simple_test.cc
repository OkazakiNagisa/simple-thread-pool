#include <catch2/catch_test_macros.hpp>
#include <stp/ThreadPool.h>

using stp::ThreadPool;

TEST_CASE("Evaluate task future correctness", "[stp]")
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

TEST_CASE("Evaluate multi tasks", "[stp]")
{
    ThreadPool tp;
    tp.WaitAll();
}
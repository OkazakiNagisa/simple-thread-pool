#include <print>
#include <source_location>
#include <stp/ThreadPool.h>

using stp::ThreadPool;

int main()
{
    std::println("hello from {}!",
                 std::source_location::current().function_name());

    auto tp = ThreadPool(std::thread::hardware_concurrency());
    auto future = tp.EnqueueTask(
        [](int a) {
            int ret = 0;
            for (int i = 0; i < a; i++)
            {
                ret += 10 * i;
            }
            return ret;
        },
        1000000);

    std::println("task returns: {}", future.get());
    tp.WaitAll();
    return 0;
}
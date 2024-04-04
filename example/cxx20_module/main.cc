// import std;
#include <print>
#include <source_location>
import stp;
using stp::ThreadPool;

int main()
{
    std::println("hello from {}!",
                 std::source_location::current().function_name());

    auto tp = ThreadPool();
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
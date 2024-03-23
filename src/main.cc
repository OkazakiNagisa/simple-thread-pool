#include <print>
#include <source_location>
#include "ThreadPool.h"

int main()
{
    auto tp = ThreadPool(4);
    // ThreadPool tn = std::move(tp);

    std::println("Hello from {}!",
                 std::source_location::current().function_name());
    return 0;
}
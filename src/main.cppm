module;

#include <print>
#include <source_location>

export module main;

export import lib;
using namespace lib;

int main()
{
    ThreadPool tp;
    std::println("Hello from {}!",
                 std::source_location::current().function_name());
    return 0;
}
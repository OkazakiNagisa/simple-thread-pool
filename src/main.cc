export module main;
import std;
import lib;
using namespace lib;

export int main()
{
    ThreadPool tp;
    std::println("Hello from {}!",
                 std::source_location::current().function_name());
    return 0;
}
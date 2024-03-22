module;

#include <print>
#include <source_location>

export module std_module;

export namespace std
{
using std::print;
using std::println;
using std::source_location;
};
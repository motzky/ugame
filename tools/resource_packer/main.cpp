#include <iostream>
#include <print>

#include "exception.h"

auto main() -> int
{
    try
    {

        std::println("resource packer!");
    }
    catch (const game::Exception &err)
    {
        std::println(std::cerr, "{}", err);
    }
    catch (...)
    {
        std::println(std::cerr, "unknown exception");
    }

    return 0;
}
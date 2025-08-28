#include <iostream>
#include <print>
#include <auto_release.h>

#include "error.h"
#include "exception.h"
#include "window.h"

auto main() -> int
{
    std::println("Hello, World!\n");

    try
    {
        // game::Window window(800u, 600u); // Create a window with specified width and height
        game::ensure(false, "test ensure");
    }
    catch (const game::Exception &err)
    {
        std::println(std::cerr, "{}\n{}", err.what(), err.stacktrace());
    }
    catch (...)
    {
        std::println(std::cerr, "unknown exception");
    }

    return 0;
}
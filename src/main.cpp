#include <print>
#include <auto_release.h>

#include "window.h"

auto main() -> int
{
    std::println("Hello, World!\n");

    game::Window window(800u, 600u); // Create a window with specified width and height

    return 0;
}
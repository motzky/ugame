#include <iostream>
#include <print>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "config.h"
#include "game/game.h"
#include "utils/exception.h"

auto main(int argc, char **argv) -> int
{
    game::log::info("starting game {}.{}.{}", game::version::major, game::version::minor, game::version::patch);

    const auto args = std::vector<std::string_view>(argv + 1u, argv + argc);
    game::log::info("args: {}", args);

    {
        auto g = game::Game{args};

        try
        {
            const auto root = !args.empty() ? args.front() : ".";
            g.run(root);
        }
        catch (const game::Exception &err)
        {
            std::println(std::cerr, "{}", err);
        }
        catch (...)
        {
            std::println(std::cerr, "unknown exception");
        }
    }

#ifndef WIN32
    ::glfwTerminate();
#endif

    return 0;
}
#include <iostream>
#include <print>

#include "config.h"
#include "game/game.h"
#include "utils/ensure.h"
#include "utils/exception.h"

auto main(int argc, char **argv) -> int
{
    game::log::info("starting game {}.{}.{}", game::version::major, game::version::minor, game::version::patch);

    {
        auto g = game::Game{1920u, 1080u};

        try
        {
            game::ensure(argc >= 2, "{} <root_path>", argv[0]);

            g.run(argv[1]);
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
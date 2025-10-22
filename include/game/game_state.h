#pragma once

#include <format>
#include <string>
#include <utility>

namespace game
{
    enum class GameState
    {
        MAIN_MENU,
        RUNNING,
        PAUSED,
        EXITING
    };

    inline auto to_string(GameState obj) -> std::string
    {
        switch (obj)
        {
            using enum GameState;
        case MAIN_MENU:
            return "MAIN_MENU";
        case RUNNING:
            return "RUNNING";
        case PAUSED:
            return "PAUSED";
        case EXITING:
            return "EXITING";
        default:
            return std::format("{}", std::to_underlying(obj));
        }
    }

}

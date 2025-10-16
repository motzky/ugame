#pragma once

#include <format>
#include <string>
#include <utility>

namespace game
{
    enum class GameState
    {
        RUNNING,
        PAUSED,
        EXITING
    };

    inline auto to_string(GameState obj) -> std::string
    {
        switch (obj)
        {
            using enum GameState;
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

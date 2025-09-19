#pragma once

#include "game/player.h"
#include "graphics/scene.h"

namespace game::levels
{
    class Level
    {
    public:
        virtual ~Level() = default;

        virtual auto update(const Player &player) -> void = 0;
        virtual auto restart() -> void = 0;

        auto scene() -> Scene &
        {
            return _scene;
        }

    protected:
        Scene _scene;
    };
}
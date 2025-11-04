#pragma once

#include "game/player.h"
#include "graphics/scene.h"

namespace game::levels
{
    class Level
    {
    public:
        virtual ~Level() = default;

        virtual auto update(Player &player) -> void = 0;
        virtual auto restart() -> void = 0;

        auto set_show_debug(bool show_debug) -> void { _show_debug = show_debug; }
        auto set_show_physics_debug(bool show_physics_debug) -> void { _show_physics_debug = show_physics_debug; }

        auto scene() -> Scene &
        {
            return _scene;
        }

    protected:
        Scene _scene;
        bool _show_debug = false;
        bool _show_physics_debug = false;
    };
}
#pragma once

#include <string_view>

#include "events/key_event.h"
#include "events/mouse_button_event.h"
#include "events/mouse_event.h"
#include "game/game_state.h"
#include "log.h"

namespace game
{
    class Camera;
    class Entity;
    struct Scene;
}

namespace game::messaging
{

    class Subscriber
    {
    public:
        virtual ~Subscriber() = default;

        virtual auto handle_key_press(const KeyEvent &) -> void
        {
            log::error("unhandled message");
        };

        virtual auto handle_mouse_move(const MouseEvent &) -> void
        {
            log::error("unhandled message");
        };

        virtual auto handle_mouse_button(const MouseButtonEvent &) -> void
        {
            log::error("unhandled message");
        };
        virtual auto handle_level_complete(const std::string_view &) -> void
        {
            log::error("unhandled message");
        };
        virtual auto handle_entity_intersect(const game::Entity *, const game::Entity *) -> void
        {
            log::error("unhandled message");
        };
        virtual auto handle_restart_level() -> void
        {
            log::error("unhandled message");
        };
        virtual auto handle_quit() -> void
        {
            log::error("unhandled message");
        };
        virtual auto handle_state_change(GameState) -> void
        {
            log::error("unhandled message");
        }
        virtual auto handle_change_camera(const Camera *) -> void
        {
            log::error("unhandled message");
        }
        virtual auto handle_change_scene(Scene *) -> void
        {
            log::error("unhandled message");
        }

    private:
    };
}
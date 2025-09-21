#pragma once

#include <string_view>

#include "events/key_event.h"
#include "log.h"

namespace game::messaging
{
    class Subscriber
    {
    public:
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

    private:
    };
}
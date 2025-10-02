#pragma once

#include <unordered_map>
#include <vector>

#include "events/key_event.h"
#include "events/mouse_button_event.h"
#include "events/mouse_event.h"

namespace game
{
    class Entity;
}

namespace game::messaging
{
    class Subscriber;

    enum class MessageType
    {
        MOUSE_MOVE,
        MOUSE_BUTTON_PRESS,
        MOUSE_SCROLL,
        KEY_PRESS,
        LEVEL_COMPLETE,
        ENTITY_INTERSECT,
        RESTART_LEVEL,
    };

    class MessageBus
    {
    public:
        auto subscribe(MessageType type, Subscriber *subscriber) -> void;

        auto post_key_press(const KeyEvent &event) -> void;
        auto post_mouse_move(const MouseEvent &event) -> void;
        auto post_mouse_button(const MouseButtonEvent &event) -> void;
        auto post_level_complete(const std::string_view &level_name) -> void;
        auto post_entity_intersect(const game::Entity *a, const game::Entity *b) -> void;
        auto post_restart_level() -> void;

    private:
        std::unordered_map<MessageType, std::vector<Subscriber *>> _subscribers;
    };
}
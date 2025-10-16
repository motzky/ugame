#pragma once

#include <unordered_map>
#include <vector>

#include "events/key_event.h"
#include "events/mouse_button_event.h"
#include "events/mouse_event.h"
#include "game/game_state.h"

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
        QUIT,
        STATE_CHANGE,
    };

    class MessageBus
    {
    public:
        auto subscribe(MessageType type, Subscriber *subscriber) -> void;
        auto unsubscribe(MessageType type, Subscriber *subscriber) -> void;

        MessageBus() = default;
        ~MessageBus() = default;
        MessageBus(const MessageBus &) = delete;
        auto operator=(const MessageBus &) -> MessageBus & = delete;
        MessageBus(MessageBus &&) = default;
        auto operator=(MessageBus &&) -> MessageBus & = default;

        auto post_key_press(const KeyEvent &event) -> void;
        auto post_mouse_move(const MouseEvent &event) -> void;
        auto post_mouse_button(const MouseButtonEvent &event) -> void;
        auto post_level_complete(const std::string_view &level_name) -> void;
        auto post_entity_intersect(const game::Entity *a, const game::Entity *b) -> void;
        auto post_restart_level() -> void;
        auto post_quit() -> void;
        auto post_state_change(GameState state) -> void;

    private:
        std::unordered_map<MessageType, std::vector<Subscriber *>> _subscribers;
    };
}
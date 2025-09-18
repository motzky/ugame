#pragma once

#include "events/key_event.h"
#include <any>
#include <unordered_map>
#include <vector>

namespace game::messaging
{
    class Subscriber;

    enum class MessageType
    {
        MOUSE_MOVE,
        MOUSE_BUTTON_PRESS,
        MOUSE_SCROLL,
        KEY_PRESS,

    };

    class MessageBus
    {
    public:
        auto subscribe(MessageType type, Subscriber *subscriber) -> void;

        auto post_key_press(const KeyEvent &event) -> void;

    private:
        std::unordered_map<MessageType, std::vector<Subscriber *>> _subscribers;
    };
}
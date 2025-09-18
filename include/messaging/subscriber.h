#pragma once

#include "messaging/message_bus.h"
#include "messaging/subscriber.h"

namespace game::messaging
{
    class Subscriber
    {
    public:
        virtual auto handle_key_press(const KeyEvent &) -> void {};

    private:
    };
}
#pragma once

#include "events/key_event.h"

namespace game::messaging
{
    class Subscriber
    {
    public:
        virtual auto handle_key_press(const KeyEvent &) -> void {};

    private:
    };
}
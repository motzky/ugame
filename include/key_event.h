#pragma once

#include "key.h"

namespace game
{

    class KeyEvent
    {

    public:
        KeyEvent(Key key, KeyState state);
        auto key() const -> Key;
        auto state() const -> KeyState;

    private:
        Key _key;
        KeyState _state;
    };
}

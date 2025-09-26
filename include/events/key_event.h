#pragma once

#include "core/key.h"

namespace game
{

    class KeyEvent
    {

    public:
        KeyEvent(Key key, KeyState state);
        auto key() const -> Key;
        auto state() const -> KeyState;

        auto operator==(const KeyEvent &) const -> bool = default;
        auto to_string() const -> std::string;

    private:
        Key _key;
        KeyState _state;
    };
}

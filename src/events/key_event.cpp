#include "events/key_event.h"

#include <format>

#include "core/key.h"
#include "utils/formatter.h"

namespace game
{

    KeyEvent::KeyEvent(Key key, KeyState state)
        : _key(key), _state(state)
    {
    }
    auto KeyEvent::key() const -> Key
    {
        return _key;
    }

    auto KeyEvent::state() const -> KeyState
    {
        return _state;
    }

    auto KeyEvent::to_string() const -> std::string
    {
        return std::format("KeyEvent {} {}", _key, _state);
    }

}

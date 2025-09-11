#include "events/key_event.h"

#include "key.h"

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
}

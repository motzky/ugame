#include "events/mouse_button_event.h"

namespace game
{

    MouseButtonEvent::MouseButtonEvent(float x, float y, MouseButtonState state)
        : _x(x), _y(y), _state(state)
    {
    }

    auto MouseButtonEvent::x() const -> float
    {
        return _x;
    }
    auto MouseButtonEvent::y() const -> float
    {
        return _y;
    }
    auto MouseButtonEvent::state() const -> MouseButtonState
    {
        return _state;
    }
}

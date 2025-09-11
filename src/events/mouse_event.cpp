#include "events/mouse_event.h"

namespace game
{

    MouseEvent::MouseEvent(float delta_x, float delta_y)
        : _delta_x(delta_x), _delta_y(delta_y)
    {
    }

    auto MouseEvent::delta_x() const -> float
    {
        return _delta_x;
    }
    auto MouseEvent::delta_y() const -> float
    {
        return _delta_y;
    }
}

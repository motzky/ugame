#pragma once

namespace game
{

    class MouseEvent
    {

    public:
        MouseEvent(float delta_x, float delta_y);

        auto delta_x() const -> float;
        auto delta_y() const -> float;

    private:
        float _delta_x;
        float _delta_y;
    };
}

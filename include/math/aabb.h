#pragma once

#include "math/vector3.h"

namespace game
{
    class AABB
    {
    public:
        AABB() = default;
        AABB(Vector3 min, Vector3 max, Vector3 pos = {});

        auto get_min() const -> Vector3;
        auto get_max() const -> Vector3;

        auto set_position(Vector3 new_position) -> void;

    private:
        Vector3 _min;
        Vector3 _max;
        Vector3 _pos;
    };

}

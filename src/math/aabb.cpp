#include "math/aabb.h"

namespace game
{
    AABB::AABB(Vector3 min, Vector3 max, Vector3 pos)
        : _min{min}, _max{max}, _pos{pos}
    {
    }

    auto AABB::get_min() const -> Vector3
    {
        return _pos - _min;
    }

    auto AABB::get_max() const -> Vector3
    {
        return _pos + _max;
    }

    auto AABB::set_position(Vector3 new_position) -> void
    {
        _pos = new_position;
    }

}
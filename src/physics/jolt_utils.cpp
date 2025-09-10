#include "physics/jolt_utils.h"

namespace game
{
    auto to_native(::JPH::RVec3Arg v) -> game::Vector3
    {
        return {v.GetX(), v.GetY(), v.GetZ()};
    }

    auto to_native(::JPH::ColorArg c) -> game::Color
    {
        const auto v = c.ToVec4();
        return {v.GetX(), v.GetY(), v.GetZ()};
    }

    auto to_jolt(const Vector3 &v) -> ::JPH::RVec3
    {
        return {v.x, v.y, v.z};
    }

    // auto to_jolt(const Color &c) -> ::JPH::Color
    // {
    //     return ::JPH::Color();
    // }

}
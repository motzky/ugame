#pragma once

#include <cmath>
#include <format>

#include "math/vector3.h"

namespace game
{
    class Quaternion
    {
    public:
        constexpr Quaternion()
            : Quaternion(0.f, 0.f, 0.f, 1.f)
        {
        }

        constexpr Quaternion(float x, float y, float z, float w)
            : x{x},
              y{y},
              z{z},
              w{w}
        {
        }

        constexpr Quaternion(const Vector3 &v, float w)
            : x{v.x},
              y{v.y},
              z{v.z},
              w{w}
        {
        }

        float x;
        float y;
        float z;
        float w;
    };
}

template <>
struct std::formatter<game::Quaternion>
{
    constexpr auto parse(std::format_parse_context &ctx)
    {
        return std::begin(ctx);
    }

    auto format(const game::Quaternion &obj, std::format_context &ctx) const
    {
        return std::format_to(ctx.out(), "x={} y={} z={} w={}",
                              obj.x,
                              obj.y,
                              obj.z,
                              obj.y);
    }
};

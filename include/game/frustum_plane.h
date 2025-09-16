#pragma once

#include "math/vector3.h"
#include <format>

namespace game
{
    struct FrustumPlane
    {
        FrustumPlane() = default;

        FrustumPlane(const game::Vector3 &p1, const game::Vector3 &norm)
            : normal(Vector3::normalize(norm)),
              distance(Vector3::dot(normal, p1))
        {
        }
        Vector3 normal;
        float distance;
    };
}

template <>
struct std::formatter<game::FrustumPlane>
{
    constexpr auto parse(std::format_parse_context &ctx)
    {
        return std::begin(ctx);
    }

    auto format(const game::FrustumPlane &obj, std::format_context &ctx) const
    {
        return std::format_to(ctx.out(), "normal={} distance={}",
                              obj.normal,
                              obj.distance);
    }
};

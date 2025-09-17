#pragma once

#include <format>

#include "math/matrix3.h"
#include "math/vector3.h"

namespace game
{
    struct FrustumPlane
    {
        FrustumPlane() = default;

        FrustumPlane(const game::Vector3 &p1, const Vector3 &norm)
            : normal(Vector3::normalize(norm)),
              distance(Vector3::dot(normal, p1))
        {
        }

        FrustumPlane(float a, float b, float c, float d)
        {
            const auto inv_length = 1.0f / std::hypot(a, b, c);
            normal = Vector3(a * inv_length, b * inv_length, c * inv_length);
            distance = d * inv_length;
        }

        Vector3 normal;
        float distance;
    };

    inline auto intersection(const FrustumPlane &p1, const FrustumPlane &p2, const FrustumPlane &p3) -> Vector3
    {
        const auto m = Matrix3{p1.normal, p2.normal, p3.normal};
        const auto v = Vector3{-p1.distance, -p2.distance, -p3.distance};

        return Matrix3::invert(Matrix3::transpose(m)) * v;
    }
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

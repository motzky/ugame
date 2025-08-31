#pragma once

#include <cmath>
#include <format>

#include "log.h"

namespace game
{
    struct Vector3
    {
        float x;
        float y;
        float z;

        constexpr auto length() const -> float
        {
            return std::hypot(x, y, z);
        }

        static auto normalize(const Vector3 &v) -> Vector3
        {
            const auto length = v.length();

            if (length == 0.f)
            {
                return {.x = 0.f, .y = 0.f, .z = 0.f};
            }

            return {.x = v.x / length, .y = v.y / length, .z = v.z / length};
        }

        static constexpr auto cross(const Vector3 &v1, const Vector3 &v2) -> Vector3
        {
            const auto i = (v1.y * v2.z) - (v1.z * v2.y);
            const auto j = (v1.x * v2.z) - (v1.z * v2.x);
            const auto k = (v1.x * v2.y) - (v1.y * v2.x);

            return {.x = i, .y = -j, .z = k};
        }

        constexpr auto operator==(const Vector3 &) const -> bool = default;
    };

    constexpr auto operator-=(Vector3 &v1, const Vector3 &v2) -> Vector3 &
    {
        v1.x -= v2.x;
        v1.y -= v2.y;
        v1.z -= v2.z;

        return v1;
    }
    constexpr auto operator-(const Vector3 &v1, const Vector3 &v2) -> Vector3
    {
        auto tmp = v1;
        return tmp -= v2;
    }
    constexpr auto operator+=(Vector3 &v1, const Vector3 &v2) -> Vector3 &
    {
        v1.x += v2.x;
        v1.y += v2.y;
        v1.z += v2.z;

        return v1;
    }
    constexpr auto operator+(const Vector3 &v1, const Vector3 &v2) -> Vector3
    {
        auto tmp = v1;
        return tmp += v2;
    }
    constexpr auto operator-(const Vector3 &v) -> Vector3
    {
        return {-v.x, -v.y, -v.z};
    }

}

template <>
struct std::formatter<game::Vector3>
{
    constexpr auto parse(std::format_parse_context &ctx)
    {
        return std::begin(ctx);
    }

    auto format(const game::Vector3 &obj, std::format_context &ctx) const
    {
        return std::format_to(ctx.out(), "{} {} {}",
                              obj.x,
                              obj.y,
                              obj.z);
    }
};

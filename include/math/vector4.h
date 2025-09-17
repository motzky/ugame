#pragma once

#include <format>

#include "log.h"
#include "math/vector3.h"

namespace game
{
    struct Vector4
    {
        constexpr Vector4()
            : Vector4(0.f)
        {
        }

        constexpr Vector4(float scalar)
            : Vector4(scalar, scalar, scalar, scalar)
        {
        }

        constexpr Vector4(float x, float y, float z, float w)
            : x(x), y(y), z(z), w(w)
        {
        }

        constexpr Vector4(const Vector3 &xyz, float w)
            : Vector4(xyz.x, xyz.y, xyz.z, w)
        {
        }

        float x;
        float y;
        float z;
        float w;

        static constexpr auto dot(const Vector4 &v1, const Vector4 &v2) -> float
        {
            return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
        }

        static auto normalize(const Vector4 &v) -> Vector4
        {
            const auto length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);

            if (length == 0.f)
            {
                return {};
            }

            return {v.x / length, v.y / length, v.z / length, v.w / length};
        }

        constexpr auto operator==(const Vector4 &) const -> bool = default;
    };

    constexpr auto operator-=(Vector4 &v1, const Vector4 &v2) -> Vector4 &
    {
        v1.x -= v2.x;
        v1.y -= v2.y;
        v1.z -= v2.z;
        v1.w -= v2.w;

        return v1;
    }
    constexpr auto operator-(const Vector4 &v1, const Vector4 &v2) -> Vector4
    {
        auto tmp = v1;
        return tmp -= v2;
    }
    constexpr auto operator+=(Vector4 &v1, const Vector4 &v2) -> Vector4 &
    {
        v1.x += v2.x;
        v1.y += v2.y;
        v1.z += v2.z;
        v1.w += v2.w;

        return v1;
    }
    constexpr auto operator+(const Vector4 &v1, const Vector4 &v2) -> Vector4
    {
        auto tmp = v1;
        return tmp += v2;
    }
    constexpr auto operator*=(Vector4 &v1, float scale) -> Vector4 &
    {
        v1.x *= scale;
        v1.y *= scale;
        v1.z *= scale;
        v1.w *= scale;

        return v1;
    }
    constexpr auto operator*(const Vector4 &v1, float scale) -> Vector4
    {
        auto tmp = v1;
        return tmp *= scale;
    }
    constexpr auto operator-(const Vector4 &v) -> Vector4
    {
        return {-v.x, -v.y, -v.z, -v.w};
    }

}

template <>
struct std::formatter<game::Vector4>
{
    constexpr auto parse(std::format_parse_context &ctx)
    {
        return std::begin(ctx);
    }

    auto format(const game::Vector4 &obj, std::format_context &ctx) const
    {
        return std::format_to(ctx.out(), "x={} y={} z={} w={}",
                              obj.x,
                              obj.y,
                              obj.z,
                              obj.w);
    }
};

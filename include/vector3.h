#pragma once

#include <cmath>

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
    constexpr auto operator-(const Vector3 &v) -> Vector3
    {
        return {-v.x, -v.y, -v.z};
    }

}
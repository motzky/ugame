#pragma once

#include <array>
#include <cstring>
#include <format>
#include <ranges>
#include <span>

#include <Jolt/Jolt.h>
#include <Jolt/Math/Mat44.h>
#include <Jolt/Math/Quat.h>

#include "ensure.h"
#include "math/quaternion.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "physics/jolt_utils.h"

namespace game
{
    class Matrix3
    {
    public:
        struct Scale
        {
        };
        // initialize to identity
        constexpr Matrix3()
            : _elements({1.f,
                         0.f,
                         0.f,
                         0.f,
                         1.f,
                         0.f,
                         0.f,
                         0.f,
                         1.f})
        {
        }

        constexpr Matrix3(const std::array<float, 9u> &elements)
            : _elements{elements}
        {
        }

        constexpr Matrix3(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3)
            : _elements(
                  {v1.x, v1.y, v1.z,
                   v2.x, v2.y, v2.z,
                   v3.x, v3.y, v3.z})
        {
        }

        Matrix3(const std::span<const float> &elements)
            : Matrix3{}
        {
            ensure(elements.size() == 9u, "not enough elements");
            std::memcpy(_elements.data(), elements.data(), elements.size_bytes());
        }

        constexpr auto data() const -> std::span<const float>
        {
            return _elements;
        }

        auto operator[](this auto &&self, std::size_t index) -> auto
        {
            return self._elements[index];
        }

        auto row(std::size_t index) const -> Vector3
        {
            ensure(index < 3, "index out of range");

            return {_elements[index], _elements[index + 3u], _elements[index + 6u]};
        }

        friend constexpr auto operator*=(Matrix3 &m1, const Matrix3 &m2) -> Matrix3 &;

        constexpr auto operator==(const Matrix3 &) const -> bool = default;

        static constexpr auto determinate(const Matrix3 &m) -> float
        {
            return m[0] * m[4] * m[8] +
                   m[1] * m[5] * m[6] +
                   m[2] * m[3] * m[7] -

                   m[0] * m[5] * m[7] -
                   m[1] * m[3] * m[8] -
                   m[2] * m[4] * m[6];
        }

        static constexpr auto invert(const Matrix3 &m) -> Matrix3
        {
            // 0 3 6
            // 1 4 7
            // 2 5 8

            const auto adjoint = Matrix3{{(m[4] * m[8]) - (m[5] * m[7]),
                                          -(m[1] * m[8]) + (m[2] * m[7]),
                                          (m[1] * m[5]) - (m[2] * m[4]),

                                          -(m[3] * m[8]) + (m[5] * m[6]),
                                          (m[0] * m[8]) - (m[2] * m[6]),
                                          -(m[0] * m[5]) + (m[2] * m[3]),

                                          (m[3] * m[7]) - (m[4] * m[6]),
                                          -(m[0] * m[7]) + (m[1] * m[6]),
                                          (m[0] * m[4]) - (m[1] * m[3])}};

            const auto det = determinate(m);

            const auto inv_vals = adjoint.data() | std::views::transform([det](auto e)
                                                                         { return e / det; });
            auto inv_arr = std::array<float, 9u>{};
            std::ranges::copy(inv_vals, std::ranges::begin(inv_arr));

            return {inv_arr};
        }

    private:
        std::array<float, 9u>
            _elements;
    };

    constexpr auto operator*=(Matrix3 &m1, const Matrix3 &m2) -> Matrix3 &
    {
        auto result = Matrix3{};
        for (auto i = 0u; i < 3u; i++)
        {
            for (auto j = 0u; j < 3u; j++)
            {
                result._elements[i + j * 3] = 0.f;
                for (auto k = 0u; k < 3u; ++k)
                {
                    result._elements[i + j * 3] += m1._elements[i + k * 3] * m2._elements[k + j * 3];
                }
            }
        }

        m1 = result;
        return m1;
    }

    constexpr auto operator*(const Matrix3 &m1, const Matrix3 &m2) -> Matrix3
    {
        auto tmp{m1};
        return tmp *= m2;
    }

    constexpr auto operator*(const Matrix3 &m, const Vector3 &v) -> Vector3
    {
        const auto x = m[0] * v.x + m[3] * v.y + m[6] * v.z;
        const auto y = m[1] * v.x + m[4] * v.y + m[7] * v.z;
        const auto z = m[2] * v.x + m[5] * v.y + m[8] * v.z;

        return {x, y, z};
    }
}

template <>
struct std::formatter<game::Matrix3>
{
    constexpr auto parse(std::format_parse_context &ctx)
    {
        return std::begin(ctx);
    }

    auto format(const game::Matrix3 &obj, std::format_context &ctx) const
    {
        const auto *data = obj.data().data();
        return std::format_to(ctx.out(), "{} {} {}\n{} {} {}\n{} {} {}",
                              data[0], data[3], data[6],
                              data[1], data[4], data[7],
                              data[2], data[5], data[8]);
    }
};

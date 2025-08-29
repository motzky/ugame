#pragma once

#include <array>
#include <format>
#include <span>

#include "vector3.h"

namespace game
{
    class Matrix4
    {
    public:
        // initialize to identity
        constexpr Matrix4()
            : _elements({1.f,
                         0.f,
                         0.f,
                         0.f,
                         0.f,
                         1.f,
                         0.f,
                         0.f,
                         0.f,
                         0.f,
                         1.f,
                         0.f,
                         0.f,
                         0.f,
                         0.f,
                         1.f})
        {
        }

        constexpr Matrix4(const std::array<float, 16u> &elements)
            : _elements(elements)
        {
        }

        constexpr Matrix4(const Vector3 &translation)
            : _elements({1.f,
                         0.f,
                         0.f,
                         0.f,
                         0.f,
                         1.f,
                         0.f,
                         0.f,
                         0.f,
                         0.f,
                         1.f,
                         0.f,
                         translation.x,
                         translation.y,
                         translation.z,
                         1.f})
        {
        }

        static auto look_at(const Vector3 &eye, const Vector3 &look_at, const Vector3 &up) -> Matrix4;

        auto make_translate(const game::Vector3 &v1) -> game::Matrix4
        {
            return {v1};
        }

        constexpr auto
        data() const -> std::span<const float>
        {
            return _elements;
        }

        friend constexpr auto operator*=(Matrix4 &m1, const Matrix4 &m2) -> Matrix4 &;

    private:
        std::array<float, 16u> _elements;
    };

    constexpr auto operator*=(Matrix4 &m1, const Matrix4 &m2) -> Matrix4 &
    {
        auto result = Matrix4{};
        for (auto i = 0u; i < 4u; i++)
        {
            for (auto j = 0u; j < 4u; j++)
            {
                result._elements[i + j * 4] = 0.f;
                for (auto k = 0u; k < 4u; ++k)
                {
                    result._elements[i + j * 4] += m1._elements[i + k * 4] * m2._elements[k + j * 4];
                }
            }
        }

        m1 = result;
        return m1;
    }

    constexpr auto operator*(const Matrix4 &m1, const Matrix4 &m2) -> Matrix4
    {
        auto tmp{m1};
        return tmp *= m2;
    }

    inline auto Matrix4::look_at(const Vector3 &eye, const Vector3 &look_at, const Vector3 &up) -> Matrix4
    {
        const auto f = Vector3::normalize(look_at - eye);
        const auto up_normalized = Vector3::normalize(up);

        const auto s = Vector3::normalize(Vector3::cross(f, up_normalized));
        const auto u = Vector3::normalize(Vector3::cross(s, f));

        auto m = Matrix4{};
        // m._elements = {s.x, s.y, s.z, 0.f, u.x, u.y, u.z, 0.f, -f.x, -f.y, -f.z, 0.f, 0.f, 0.f, 0.f, 1.f};
        // m._elements = { s.x, s.y, s.z, 0.f,
        //                 u.x, u.y, u.z, 0.f,
        //                -f.x, -f.y, -f.z, 0.f,
        //                 0.f, 0.f, 0.f, 1.f};

        m._elements = {s.x, u.x, -f.x, 0.f,
                       s.y, u.y, -f.y, 0.f,
                       s.z, u.z, -f.z, 0.f,
                       0.f, 0.f, 0.f, 1.f};

        return m * Matrix4{-eye};
    }

}

template <>
struct std::formatter<game::Matrix4>
{
    constexpr auto parse(std::format_parse_context &ctx)
    {
        return std::begin(ctx);
    }

    auto format(const game::Matrix4 &obj, std::format_context &ctx) const
    {
        const auto *data = obj.data().data();
        return std::format_to(ctx.out(), "{} {} {} {}\n{} {} {} {}\n{} {} {} {}\n{} {} {} {}",
                              data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);
    }
};

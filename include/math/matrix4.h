#pragma once

#include <array>
#include <format>
#include <span>

#include "math/quaternion.h"
#include "math/vector3.h"

namespace game
{
    class Matrix4
    {
    public:
        struct Scale
        {
        };
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

        constexpr Matrix4(const Vector3 &translation, const Vector3 &scale)
            : _elements({scale.x,
                         0.f,
                         0.f,
                         0.f,
                         0.f,
                         scale.y,
                         0.f,
                         0.f,
                         0.f,
                         0.f,
                         scale.z,
                         0.f,
                         translation.x,
                         translation.y,
                         translation.z,
                         1.f})
        {
        }

        constexpr Matrix4(const Vector3 &scale, Scale)
            : _elements({scale.x,
                         0.f,
                         0.f,
                         0.f,
                         0.f,
                         scale.y,
                         0.f,
                         0.f,
                         0.f,
                         0.f,
                         scale.z,
                         0.f,
                         0.f,
                         0.f,
                         0.f,
                         1.f})
        {
        }

        constexpr Matrix4(const Quaternion &rotation)
            : Matrix4{}
        {
            // // Note default initalized to identity(mat4)
            float xx = rotation.x * rotation.x;
            float yy = rotation.y * rotation.y;
            float zz = rotation.z * rotation.z;
            float xy = rotation.x * rotation.y;
            float xz = rotation.x * rotation.z;
            float yz = rotation.y * rotation.z;
            float wx = rotation.w * rotation.x;
            float wy = rotation.w * rotation.y;
            float wz = rotation.w * rotation.z;

            _elements[0] = 1.0f - 2.0f * (yy + zz);
            _elements[1] = 2.0f * (xy - wz);
            _elements[2] = 2.0f * (xz + wy);

            _elements[4] = 2.0f * (xy + wz);
            _elements[5] = 1.0f - 2.0f * (xx + zz);
            _elements[6] = 2.0f * (yz - wx);

            _elements[8] = 2.0f * (xz - wy);
            _elements[9] = 2.0f * (yz + wx);
            _elements[10] = 1.0f - 2.0f * (xx + yy);
        }

        static auto look_at(const Vector3 &eye, const Vector3 &look_at, const Vector3 &up) -> Matrix4;
        static auto perspective(float fov, float width, float height, float near_plane, float far_plane) -> Matrix4;

        auto make_translate(const game::Vector3 &v1) -> game::Matrix4
        {
            return {v1};
        }

        constexpr auto data() const -> std::span<const float>
        {
            return _elements;
        }

        auto operator[](this auto &&self, std::size_t index) -> float &
        {
            return self._elements[index];
        }

        friend constexpr auto operator*=(Matrix4 &m1, const Matrix4 &m2) -> Matrix4 &;

        constexpr auto operator==(const Matrix4 &) const -> bool = default;

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

    inline auto Matrix4::perspective(float fov, float width, float height, float near_plane, float far_plane) -> Matrix4
    {
        Matrix4 m;

        const auto aspect_ratio = width / height;
        const auto tmp = std::tanf(fov / 2.f);
        const auto t = tmp * near_plane;
        const auto b = -t;
        const auto r = t * aspect_ratio;
        const auto l = b * aspect_ratio;

        m._elements = {{(2.f * near_plane) / (r - l), 0.f, 0.f, 0.f,
                        0.f, (2.f * near_plane) / (t - b), 0.f, 0.f,
                        (r + l) / (r - l), (t + b) / (t - b), -(far_plane + near_plane) / (far_plane - near_plane), -1.f,
                        0.f, 0.f, -(2.f * far_plane * near_plane) / (far_plane - near_plane), 0.f}};

        return m;
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
                              data[0], data[4], data[8], data[12],
                              data[1], data[5], data[9], data[13],
                              data[2], data[6], data[10], data[14],
                              data[3], data[7], data[11], data[15]);
    }
};

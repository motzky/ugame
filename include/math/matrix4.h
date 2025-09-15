#pragma once

#include <array>
#include <cstring>
#include <format>
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

        Matrix4(const std::span<const float> &elements)
            : Matrix4{}
        {
            ensure(elements.size() == 16u, "not enough elements");
            std::memcpy(_elements.data(), elements.data(), elements.size_bytes());
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
            // Note default initalized to identity(mat4)
            auto x = rotation.x;
            auto y = rotation.y;
            auto z = rotation.z;
            auto w = rotation.w;

            auto tx = x + x;
            auto ty = y + y;
            auto tz = z + z;

            auto xx = tx * x;
            auto yy = ty * y;
            auto zz = tz * z;
            auto xy = tx * y;
            auto xz = tx * z;
            auto xw = tx * w;
            auto yz = ty * z;
            auto yw = ty * w;
            auto zw = tz * w;

            _elements[0] = (1.f - yy) - zz;
            _elements[1] = xy + zw;
            _elements[2] = xz - yw;

            _elements[4] = xy - zw,
            _elements[5] = (1.f - zz) - xx;
            _elements[6] = yz + xw;

            _elements[8] = xz + yw;
            _elements[9] = yz - xw;
            _elements[10] = (1.f - xx) - yy;
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

        auto row(std::size_t index) const -> Vector4
        {
            ensure(index < 4, "index out of range");

            return {_elements[index], _elements[index + 4u], _elements[index + 8u], _elements[index + 12u]};
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

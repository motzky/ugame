#pragma once

#include <array>
#include <span>

#include "graphics/frustum_plane.h"
#include "math/matrix4.h"
#include "math/vector3.h"

namespace game
{
    class Camera
    {
    public:
        Camera(const Vector3 &postion, const Vector3 &look_at, const Vector3 &up,
               float fov, float width, float height, float near_plane, float far_plane);

        auto direction() const -> Vector3;
        auto right() const -> Vector3;
        auto up() const -> Vector3;
        auto position() const -> Vector3;
        auto set_position(const Vector3 &position) -> void;

        auto translate(const Vector3 &translation) -> void;
        auto view() const -> std::span<const float>;
        auto projection() const -> std::span<const float>;

        auto adjust_pitch(float adjust) -> void;
        auto adjust_yaw(float adjust) -> void;

        auto fov() const -> float;
        auto width() const -> float;
        auto height() const -> float;
        auto near_plane() const -> float;
        auto far_plane() const -> float;

        auto frustum_corners() const -> std::array<Vector3, 8u>;
        auto frustum_planes() const -> std::array<game::FrustumPlane, 6u>;

        // auto invert_pitch(bool invert) -> void;
        // auto invert_yaw(bool invert) -> void;

        auto update() -> void;

    private:
        Matrix4 _view;
        Matrix4 _projection;
        Vector3 _position;
        Vector3 _direction;
        Vector3 _up;
        Vector3 _right;
        float _pitch;
        float _yaw;
        float _fov;
        float _width;
        float _height;
        float _near_plane;
        float _far_plane;

        // bool _invert_pitch;
        // bool _invert_yaw;

        auto recalculate_view() -> void;
    };
}

#pragma once

#include <span>

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

        auto translate(const Vector3 &translation) -> void;
        auto view() const -> std::span<const float>;
        auto projection() const -> std::span<const float>;

        auto adjust_pitch(float adjust) -> void;
        auto adjust_yaw(float adjust) -> void;

        // auto invert_pitch(bool invert) -> void;
        // auto invert_yaw(bool invert) -> void;

        auto update() -> void;

    private:
        Matrix4 _view;
        Matrix4 _projection;
        Vector3 _position;
        Vector3 _direction;
        Vector3 _right;
        Vector3 _up;
        float _pitch;
        float _yaw;

        // bool _invert_pitch;
        // bool _invert_yaw;

        auto recalculate_view() -> void;
    };
}

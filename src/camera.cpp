#include "camera.h"

#include <cmath>
#include <numbers>
#include <span>

#include "log.h"
#include "matrix4.h"
#include "vector3.h"

namespace
{
    auto create_direction(float pitch, float yaw) -> game::Vector3
    {
        return game::Vector3::normalize({std::sin(yaw) * std::cos(pitch),
                                         std::sin(pitch),
                                         std::cos(yaw) * std::cos(pitch)});
    }

}

namespace game
{
    Camera::Camera(const Vector3 &position, const Vector3 &look_at, const Vector3 &up,
                   float fov, float width, float height, float near_plane, float far_plane)
        : _view(Matrix4::look_at(position, look_at, up)), _projection(Matrix4::perspective(fov, width, height, near_plane, far_plane)), _position(position), _direction(look_at), _up(up), _pitch{}, _yaw{}
    {
    }

    auto Camera::direction() const -> Vector3
    {
        return _direction;
    }
    auto Camera::right() const -> Vector3
    {
        return _right;
    }
    auto Camera::up() const -> Vector3
    {
        return _up;
    }

    auto Camera::translate(const Vector3 &translation) -> void
    {
        _position += translation;
        _direction += translation;
    }

    auto Camera::view() const -> std::span<const float>
    {
        return _view.data();
    }

    auto Camera::projection() const -> std::span<const float>
    {
        return _projection.data();
    }

    auto Camera::adjust_pitch(float adjust) -> void
    {
        // if (_invert_pitch)
        // {
        //     _pitch -= adjust;
        //     return;
        // }
        _pitch += adjust;
    }
    auto Camera::adjust_yaw(float adjust) -> void
    {
        // if (_invert_yaw)
        // {
        //     _yaw -= adjust;
        //     return;
        // }
        _yaw += adjust;
    }

    // auto Camera::invert_pitch(bool invert) -> void
    // {
    //     _invert_pitch = invert;
    // }
    // auto Camera::invert_yaw(bool invert) -> void
    // {
    //     _invert_yaw = invert;
    // }

    auto Camera::update() -> void
    {
        _direction = create_direction(_pitch, _yaw);
        _right = Vector3::normalize(Vector3::cross(_direction, _up));
        recalculate_view();
    }

    auto Camera::recalculate_view() -> void
    {
        _view = Matrix4::look_at(_position, _position + _direction, _up);
    }
}
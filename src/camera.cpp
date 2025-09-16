#include "camera.h"

#include <cmath>
#include <numbers>
#include <print>
#include <span>

#include "log.h"
#include "math/vector3.h"

namespace
{
    auto create_direction(float pitch, float yaw) -> game::Vector3
    {
        return game::Vector3::normalize({std::cos(yaw) * std::cos(pitch),
                                         std::sin(pitch),
                                         std::sin(yaw) * std::cos(pitch)});
    }

}

namespace game
{
    Camera::Camera(const Vector3 &position, const Vector3 &look_at, const Vector3 &up,
                   float fov, float width, float height, float near_plane, float far_plane)
        : _view(Matrix4::look_at(position, look_at, up)),
          _projection(Matrix4::perspective(fov, width, height, near_plane, far_plane)),
          _position(position),
          _direction(look_at),
          _up(up),
          _right(),
          _pitch{},
          _yaw{-std::numbers::pi_v<float> / 2.f},
          _fov(fov),
          _width(width),
          _height(height),
          _near_plane(near_plane),
          _far_plane(far_plane)
    {
        _direction = create_direction(_pitch, _yaw);
        _right = Vector3::normalize(Vector3::cross(_direction, _up));
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
    auto Camera::position() const -> Vector3
    {
        return _position;
    }

    auto Camera::set_position(const Vector3 &position) -> void
    {
        _position = position;
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

    auto Camera::fov() const -> float
    {
        return _fov;
    }

    auto Camera::width() const -> float
    {
        return _width;
    }

    auto Camera::height() const -> float
    {
        return _height;
    }

    auto Camera::near_plane() const -> float
    {
        return _near_plane;
    }

    auto Camera::far_plane() const -> float
    {
        return _far_plane;
    }

    auto Camera::recalculate_view() -> void
    {
        _view = Matrix4::look_at(_position, _position + _direction, _up);
    }

    auto Camera::calculate_frustum_planes() const -> std::array<FrustumPlane, 6u>
    {
        auto planes = std::array<FrustumPlane, 6u>{};

        const float half_v_side = _far_plane * std::tan(_fov * .5f);
        const float half_h_side = half_v_side * _width / _height;
        const auto front_mult_far = _far_plane * _direction;

        return {{{_position + _near_plane * _direction, _direction},                        // 0: near
                 {_position + front_mult_far, -_direction},                                 // 1: far
                 {_position, Vector3::cross(front_mult_far - _right * half_h_side, _up)},   // 2: right
                 {_position, Vector3::cross(_up, front_mult_far + _right * half_h_side)},   // 3: left
                 {_position, Vector3::cross(_right, front_mult_far - _up * half_v_side)},   // 4: top
                 {_position, Vector3::cross(front_mult_far + _up * half_v_side, _right)}}}; // 5: bottom
    }

}
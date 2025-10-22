#include "graphics/camera.h"

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

    Camera::Camera(float width, float height, float depth)
        : _view{Matrix4::look_at({0.f, 0.f, 1.f}, {}, {0.f, 1.f, 0.f})},
          _projection{Matrix4::orthographic(width, height, depth)},
          _position{Vector3{0.f, 0.f, 1.f}},
          _direction{Vector3{0.f, 0.f, -1.f}},
          _up{Vector3{0.f, 1.f, 0.f}},
          _right{},
          _pitch{},
          _yaw{-std::numbers::pi_v<float> / 2.f},
          _fov(0.f),
          _width(width),
          _height(height),
          _near_plane(0.001f),
          _far_plane(depth)
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
    auto Camera::set_pitch(float value) -> void
    {
        _pitch = value;
    }
    auto Camera::set_yaw(float value) -> void
    {
        _yaw = value;
    }

    auto Camera::adjust_pitch(float value) -> void
    {
        // if (_invert_pitch)
        // {
        //     _pitch -= adjust;
        //     return;
        // }
        _pitch += value;

        clamp_pitch();
    }
    auto Camera::adjust_yaw(float value) -> void
    {
        // if (_invert_yaw)
        // {
        //     _yaw -= adjust;
        //     return;
        // }
        _yaw += value;
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

        auto world_up = Vector3{0.f, 1.0f, 0.f};
        _right = Vector3::normalize(Vector3::cross(_direction, world_up));
        _up = Vector3::normalize(Vector3::cross(_right, _direction));
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

    auto Camera::frustum_corners() const -> std::array<Vector3, 8u>
    {
        auto corners = std::array<Vector3, 8u>{};

        const auto tan_half_fov = std::tan(_fov / 2.0f);
        const auto aspect = _width / _height;

        const auto near_height = 2.0f * tan_half_fov * _near_plane;
        const auto near_width = near_height * aspect;

        const auto far_height = 2.0f * tan_half_fov * _far_plane;
        const auto far_width = far_height * aspect;

        const auto forward = Vector3::normalize(_direction);
        const auto right = Vector3::normalize(Vector3::cross(forward, _up));
        const auto up = Vector3::normalize(Vector3::cross(right, forward));

        const auto near_center = _position + _direction * _near_plane;
        corners[0] = near_center + up * (near_height / 2.0f) - right * (near_width / 2.0f);
        corners[1] = near_center + up * (near_height / 2.0f) + right * (near_width / 2.0f);
        corners[2] = near_center - up * (near_height / 2.0f) + right * (near_width / 2.0f);
        corners[3] = near_center - up * (near_height / 2.0f) - right * (near_width / 2.0f);

        const auto far_center = _position + _direction * _far_plane;
        corners[4] = far_center + up * (far_height / 2.0f) - right * (far_width / 2.0f);
        corners[5] = far_center + up * (far_height / 2.0f) + right * (far_width / 2.0f);
        corners[6] = far_center - up * (far_height / 2.0f) + right * (far_width / 2.0f);
        corners[7] = far_center - up * (far_height / 2.0f) - right * (far_width / 2.0f);

        return corners;
    }

    auto Camera::frustum_planes() const -> std::array<FrustumPlane, 6u>
    {
        const auto view_proj = _projection * _view;
        return {{
            {view_proj[3] - view_proj[2],
             view_proj[7] - view_proj[6],
             view_proj[11] - view_proj[10],
             view_proj[15] - view_proj[14]},
            {view_proj[3] + view_proj[2],
             view_proj[7] + view_proj[6],
             view_proj[11] + view_proj[10],
             view_proj[15] + view_proj[14]},
            {view_proj[3] + view_proj[0],
             view_proj[7] + view_proj[4],
             view_proj[11] + view_proj[8],
             view_proj[15] + view_proj[12]},
            {view_proj[3] - view_proj[0],
             view_proj[7] - view_proj[4],
             view_proj[11] - view_proj[8],
             view_proj[15] - view_proj[12]},
            {view_proj[3] + view_proj[1],
             view_proj[7] + view_proj[5],
             view_proj[11] + view_proj[9],
             view_proj[15] + view_proj[13]},
            {view_proj[3] - view_proj[1],
             view_proj[7] - view_proj[5],
             view_proj[11] - view_proj[9],
             view_proj[15] - view_proj[13]},
        }};
    }

    auto Camera::clamp_pitch() -> void
    {
        if (_pitch > 1.55334303f)
        {
            _pitch = 1.55334303f;
            return;
        }
        if (_pitch < -1.55334303f)
        {
            _pitch = -1.55334303f;
            return;
        }
    }

}
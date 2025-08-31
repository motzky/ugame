#include "camera.h"

#include <span>

#include "log.h"
#include "matrix4.h"
#include "vector3.h"

namespace game
{
    Camera::Camera(const Vector3 &position, const Vector3 &look_at, const Vector3 &up,
                   float fov, float width, float height, float near_plane, float far_plane)
        : _view(Matrix4::look_at(position, look_at, up)), _projection(Matrix4::perspective(fov, width, height, near_plane, far_plane)), _position(position), _direction(look_at), _up(up)
    {
    }

    auto Camera::translate(const Vector3 &translation) -> void
    {
        _position += translation;
        _direction += translation;
        _view = Matrix4::look_at(_position, _direction, _up);
    }

    auto Camera::view() const -> std::span<const float>
    {
        return _view.data();
    }

    auto Camera::projection() const -> std::span<const float>
    {
        return _projection.data();
    }
}
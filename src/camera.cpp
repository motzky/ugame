#include "camera.h"

#include <span>

#include "vector3.h"
#include "matrix4.h"

namespace game
{
    Camera::Camera(const Vector3 &postion, const Vector3 &look_at, const Vector3 &up,
                   float fov, float width, float height, float near_plane, float far_plane)
        : _view(Matrix4::look_at(postion, look_at, up)), _projection(Matrix4::perspective(fov, width, height, near_plane, far_plane))
    {
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
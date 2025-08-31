#pragma once

#include <span>

#include "vector3.h"
#include "matrix4.h"

namespace game
{
    class Camera
    {
    public:
        Camera(const Vector3 &postion, const Vector3 &look_at, const Vector3 &up,
               float fov, float width, float height, float near_plane, float far_plane);

        auto translate(const Vector3 &translation) -> void;
        auto view() const -> std::span<const float>;
        auto projection() const -> std::span<const float>;

    private:
        Matrix4 _view;
        Matrix4 _projection;
        Vector3 _position;
        Vector3 _direction;
        Vector3 _up;
    };
}

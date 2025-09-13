#include "game/camera_object_transformer.h"

#include "camera.h"
#include "game/object_transformer.h"
#include "math/vector3.h"

namespace game
{
    CameraObjectTransformer::CameraObjectTransformer(const Vector3 &position, const Camera &camera)
        : _position(position),
          _camera{camera},
          _camera_last_position(camera.position())
    {
    }

    auto CameraObjectTransformer::update() -> void
    {
        _position += _camera.position() - _camera_last_position;
        _camera_last_position = _camera.position();
    }

    auto CameraObjectTransformer::position() const -> Vector3
    {
        return _position;
    }
}
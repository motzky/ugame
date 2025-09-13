#include "game/camera_object_transformer.h"

#include "camera.h"
#include "game/object_transformer.h"
#include "math/vector3.h"

namespace game
{
    CameraObjectTransformer::CameraObjectTransformer(const Vector3 &position, const Camera &camera)
        : _position(position),
          _camera{camera}
    {
    }

    auto CameraObjectTransformer::update() -> void
    {
    }

    auto CameraObjectTransformer::position() const -> Vector3
    {
        return _camera.position() - _position;
    }
}
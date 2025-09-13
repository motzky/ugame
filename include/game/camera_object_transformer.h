#pragma once

#include "camera.h"
#include "game/object_transformer.h"
#include "math/vector3.h"

namespace game
{
    class CameraObjectTransformer : public ObjectTransformer
    {
    public:
        CameraObjectTransformer(const Vector3 &position, const Camera &camera);
        ~CameraObjectTransformer() override = default;

        virtual auto update() -> void override;
        virtual auto position() const -> Vector3 override;

    private:
        Vector3 _position;
        const Camera &_camera;
        Vector3 _camera_last_position;
    };
}
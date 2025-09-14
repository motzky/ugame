#pragma once

#include "camera.h"
#include "game/object_transformer.h"
#include "math/vector3.h"

namespace game
{
    class InverseCameraObjectTransformer : public ObjectTransformer
    {
    public:
        InverseCameraObjectTransformer(const Vector3 &position, const Camera &camera);
        ~InverseCameraObjectTransformer() override = default;

        virtual auto update() -> void override;
        virtual auto position() const -> Vector3 override;

    private:
        Vector3 _position;
        const Camera &_camera;
        Vector3 _camera_last_position;
    };
}
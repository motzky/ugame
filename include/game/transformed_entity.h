#pragma once

#include <memory>

#include "camera.h"
#include "game/chain.h"
#include "math/aabb.h"
#include "math/vector3.h"
#include "primitives/entity.h"

namespace game
{
    struct GameTransformState
    {
        const Camera &camera;
        AABB aabb;
        Vector3 camera_last_position;
    };

    struct TransformedEntity
    {
        Entity entity;
        AABB bounding_box;
        std::unique_ptr<ChainBase<GameTransformState>> transformer_chain;
    };

}
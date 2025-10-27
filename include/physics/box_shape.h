#pragma once

#include <string>
#include <vector>

#include <Jolt/Jolt.h>

#include <Jolt/Core/Reference.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>

#include "math/vector3.h"
#include "physics/shape.h"
#include "utils/pass_key.h"

namespace game
{
    class PhysicsSystem;

    class BoxShape : public Shape
    {
    public:
        BoxShape(const Vector3 &dimensions, PassKey<PhysicsSystem>);

        auto dimensions() const -> Vector3;

        virtual auto native_handle() const -> const ::JPH::Shape * override;

        auto to_string() const -> std::string;

    private:
        Vector3 _dimensions;
        ::JPH::BoxShapeSettings _shape_settings;
        ::JPH::BoxShape _shape;
    };
}

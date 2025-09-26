#pragma once

#include <Jolt/Jolt.h>

#include <Jolt/Core/Reference.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

#include "math/vector3.h"
#include "physics/physics_sytem.h"
#include "physics/shape.h"
#include "utils/pass_key.h"

namespace game
{
    class PhysicsSystem;

    class SphereShape : public Shape
    {
    public:
        SphereShape(float radius, PassKey<PhysicsSystem>);

        auto radius() const -> float;

        virtual auto native_handle() const -> const ::JPH::ShapeSettings * override;

        auto to_string() const -> std::string;

    private:
        float _radius;
        ::JPH::SphereShapeSettings _shape_settings;
    };
}

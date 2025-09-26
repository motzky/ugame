#pragma once

#include <string>

#include <Jolt/Jolt.h>

#include <Jolt/Core/Reference.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>

#include "physics/physics_sytem.h"
#include "physics/shape.h"
#include "utils/pass_key.h"

namespace game
{
    class PhysicsSystem;

    class CylinderShape : public Shape
    {
    public:
        CylinderShape(float half_height, float radius, PassKey<PhysicsSystem>);

        auto half_height() const -> float;
        auto radius() const -> float;

        virtual auto native_handle() const -> const ::JPH::ShapeSettings * override;

        auto to_string() const -> std::string;

    private:
        float _half_height;
        float _radius;
        ::JPH::CylinderShapeSettings _shape_settings;
    };
}

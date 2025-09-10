#pragma once

#include <Jolt/Jolt.h>

#include <Jolt/Core/Reference.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>

#include "pass_key.h"

namespace game
{
    class PhysicsSystem;

    class Shape
    {
    public:
        Shape(PassKey<PhysicsSystem>);

        virtual auto native_handle() const -> const ::JPH::ShapeSettings * = 0;
    };
}
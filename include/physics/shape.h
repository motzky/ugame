#pragma once

#include <Jolt/Jolt.h>

#include <Jolt/Core/Reference.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>

#include "utils/pass_key.h"

namespace game
{
    enum class ShapeType
    {
        BOX,
        CYLINDER,
        SPHERE,
        MESH
    };

    class PhysicsSystem;

    class Shape
    {
    public:
        Shape(ShapeType type, PassKey<PhysicsSystem>);
        virtual ~Shape() = default;

        virtual auto native_handle() const -> const ::JPH::Shape * = 0;

        auto type() const -> ShapeType;

    private:
        ShapeType _type;
    };
}
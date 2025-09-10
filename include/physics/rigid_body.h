#pragma once

#include <Jolt/Jolt.h>

#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyInterface.h>

#include "math/vector3.h"
#include "pass_key.h"
#include "physics/shape.h"

namespace game
{
    enum class RigidBodyType
    {
        STATIC,
        DYNAMIC
    };

    class PhysicsSystem;

    class RigidBody
    {
    public:
        RigidBody(const Shape &shape, const Vector3 &position, RigidBodyType type, ::JPH::BodyInterface &body_interface, PassKey<PhysicsSystem>);

        auto type() const -> RigidBodyType;
        auto native_handle() const -> ::JPH::Body *;

    private:
        ::JPH::Body *_body;
        RigidBodyType _type;
    };
}
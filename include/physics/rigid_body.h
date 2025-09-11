#pragma once

#include <Jolt/Jolt.h>

#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyInterface.h>

#include "math/quaternion.h"
#include "math/vector3.h"
#include "physics/shape.h"
#include "utils/pass_key.h"

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
        RigidBody(const RigidBody &) = delete;
        auto operator=(const RigidBody &) -> RigidBody & = delete;
        RigidBody(RigidBody &&) = default;
        auto operator=(RigidBody &&) -> RigidBody & = default;

        auto position() const -> Vector3;
        auto rotation() const -> Quaternion;

        auto type() const -> RigidBodyType;
        auto native_handle() const -> ::JPH::Body *;

    private:
        ::JPH::Body *_body;
        RigidBodyType _type;
        ::JPH::BodyInterface *_body_interface;
    };
}
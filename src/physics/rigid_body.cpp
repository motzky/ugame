#include "physics/rigid_body.h"

#include <Jolt/Jolt.h>

#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyInterface.h>

#include "math/vector3.h"
#include "pass_key.h"
#include "physics/jolt_utils.h"

namespace game
{
    RigidBody::RigidBody(const Shape &shape, const Vector3 &position, RigidBodyType type, ::JPH::BodyInterface &body_interface, PassKey<PhysicsSystem>)
        : _body{},
          _type(type)
    {
        const auto body_settings = ::JPH::BodyCreationSettings{
            shape.native_handle(), to_jolt(position), ::JPH::Quat::sIdentity(), to_jolt(_type), to_layer(_type)};

        _body = body_interface.CreateBody(body_settings);
        body_interface.AddBody(_body->GetID(), to_activation(_type));
    }

    auto RigidBody::type() const -> RigidBodyType
    {
        return _type;
    }

    auto RigidBody::native_handle() const -> ::JPH::Body *
    {
        return _body;
    }

}
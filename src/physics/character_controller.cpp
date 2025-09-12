#include "physics/character_controller.h"

#include <Jolt/Jolt.h>

#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>

#include <utils/pass_key.h>

namespace game
{
    CharacterController::CharacterController(::JPH::PhysicsSystem *physics_system, PassKey<PhysicsSystem>)
        : _character{}
    {
        static constexpr auto height_standing = 1.f;
        static constexpr auto radius_standing = .5f;

        ::JPH::Ref<::JPH::CharacterVirtualSettings> settings = new ::JPH::CharacterVirtualSettings();
        settings->mShape = ::JPH::RotatedTranslatedShapeSettings{
            ::JPH::Vec3(0.f, .5f * height_standing + radius_standing, 0),
            ::JPH::Quat::sIdentity(),
            new ::JPH::CapsuleShape{.5f * height_standing, radius_standing}}
                               .Create()
                               .Get();

        _character = new ::JPH::CharacterVirtual{settings, ::JPH::RVec3::sZero(), ::JPH::Quat::sIdentity(), 0, physics_system};
    }
}
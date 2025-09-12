#pragma once

#include <Jolt/Jolt.h>

#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include "utils/pass_key.h"

namespace game
{
    class PhysicsSystem;
    class CharacterController
    {
    public:
        CharacterController(::JPH::PhysicsSystem *physics_system, PassKey<PhysicsSystem>);

    private:
        ::JPH::Ref<::JPH::CharacterVirtual> _character;
    };
}
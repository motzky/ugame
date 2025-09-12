#pragma once

#include <Jolt/Jolt.h>

#include <Jolt/Physics/Character/CharacterVirtual.h>

#include <utils/pass_key.h>

namespace game
{
    class PhysicsSystem;
    class CharacterController
    {
    public:
        CharacterController(PassKey<PhysicsSystem>);

    private:
        ::JPH::Ref<::JPH::CharacterVirtual> _character;
    };
}
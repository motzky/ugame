#pragma once

#include <Jolt/Jolt.h>

#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Renderer/DebugRenderer.h>

#include "math/vector3.h"
#include "utils/pass_key.h"

namespace game
{
    class PhysicsSystem;
    class CharacterController
    {
    public:
        CharacterController(::JPH::PhysicsSystem *physics_system, PassKey<PhysicsSystem>);

        auto position() const -> Vector3;

        auto debug_draw(::JPH::DebugRenderer *debug_renderer, PassKey<PhysicsSystem>) const -> void;

    private:
        ::JPH::Ref<::JPH::CharacterVirtual> _character;
    };
}
#pragma once

#include <Jolt/Jolt.h>

#include <Jolt/Physics/Character/Character.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Renderer/DebugRenderer.h>

#include "math/vector3.h"
#include "utils/pass_key.h"

namespace game
{
    class PhysicsSystem;
    class CharacterController : public ::JPH::CharacterContactListener
    {
    public:
        CharacterController(::JPH::PhysicsSystem *physics_system, PassKey<PhysicsSystem>);

        auto position() const -> Vector3;

        auto debug_draw(::JPH::DebugRenderer *debug_renderer, PassKey<PhysicsSystem>) const -> void;

        auto set_linear_velocity(const Vector3 &velocity) -> void;

        auto update(
            float delta,
            const ::JPH::BroadPhaseLayerFilter &broad_phase_layer_filter,
            const ::JPH::ObjectLayerFilter &object_layer_filter,
            PassKey<PhysicsSystem>) -> void;

        auto OnContactAdded(
            const ::JPH::CharacterVirtual *character,
            const ::JPH::BodyID &bodyId2,
            const ::JPH::SubShapeID &subShapeId2,
            ::JPH::RVec3Arg contactPosition,
            ::JPH::Vec3Arg contactNormal,
            ::JPH::CharacterContactSettings &settings) -> void override;

    private:
        ::JPH::Ref<::JPH::CharacterVirtual> _character;
        std::unique_ptr<::JPH::TempAllocator> _tmp_allocator;
    };
}
#include "physics/character_controller.h"

#include <Jolt/Jolt.h>

#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>

#include "log.h"
#include "physics/capsule_shape.h"
#include "physics/jolt_utils.h"
#include "physics/physics_sytem.h"
#include "utils/pass_key.h"

namespace
{
    constexpr auto height_standing = 8.f;
    constexpr auto radius_standing = .66f;

}

namespace game
{
    CharacterController::CharacterController(PhysicsSystem *ps, ::JPH::PhysicsSystem *physics_system, PassKey<PhysicsSystem>)
        : _character{},
          _tmp_allocator{std::make_unique<::JPH::TempAllocatorImpl>(4 * 1025 * 1024)},
          _shape{ps->create_shape<CapsuleShape>(.5f * height_standing, radius_standing)}
    {

        ::JPH::Ref<::JPH::CharacterVirtualSettings>
            settings = new ::JPH::CharacterVirtualSettings();
        settings->mShape = ::JPH::RotatedTranslatedShapeSettings{
            ::JPH::RVec3::sZero(),
            ::JPH::Quat::sIdentity(),
            _shape->native_handle()}
                               .Create()
                               .Get();

        settings->mInnerBodyLayer = to_jolt_layer(RigidBodyType::DYNAMIC);

        _character = new ::JPH::CharacterVirtual{settings, ::JPH::Vec3(0.f, .5f * height_standing + radius_standing, 0), ::JPH::Quat::sIdentity(), 0, physics_system};
        _character->SetListener(this);
    }

    auto CharacterController::position() const -> Vector3
    {
        return to_native(_character->GetPosition());
    }

    auto CharacterController::set_position(Vector3 position) -> void
    {
        _character->SetPosition(to_jolt(position));
    }

    auto CharacterController::debug_draw(::JPH::DebugRenderer *debug_renderer, PassKey<PhysicsSystem>) const -> void
    {
        const auto transform = _character->GetCenterOfMassTransform();
        return _character->GetShape()->Draw(debug_renderer, transform, ::JPH::Vec3::sOne(), ::JPH::Color::sGreen, false, true);
    }

    auto CharacterController::set_linear_velocity(const Vector3 &velocity) -> void
    {
        _character->SetLinearVelocity(to_jolt(velocity));
    }

    auto CharacterController::update(
        float delta,
        const ::JPH::BroadPhaseLayerFilter &broad_phase_layer_filter,
        const ::JPH::ObjectLayerFilter &object_layer_filter,
        PassKey<PhysicsSystem>) -> void
    {
        _character->Update(
            delta,
            ::JPH::Vec3{0.f, -9.80665f, 0.f},
            broad_phase_layer_filter,
            object_layer_filter,
            {},
            {},
            *_tmp_allocator);
    }

    auto CharacterController::OnContactAdded(
        [[maybe_unused]] const ::JPH::CharacterVirtual *character,
        [[maybe_unused]] const ::JPH::BodyID &bodyId2,
        [[maybe_unused]] const ::JPH::SubShapeID &subShapeId2,
        [[maybe_unused]] ::JPH::RVec3Arg contactPosition,
        [[maybe_unused]] ::JPH::Vec3Arg contactNormal,
        [[maybe_unused]] ::JPH::CharacterContactSettings &settings) -> void
    {
        log::debug("contact: {}", bodyId2.GetIndex());
    }

    auto CharacterController::shape() const -> const Shape *
    {
        return _shape;
    }

}
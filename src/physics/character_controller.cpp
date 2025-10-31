#include "physics/character_controller.h"

#include <Jolt/Jolt.h>

#include <Jolt/Renderer/DebugRenderer.h>

#include "log.h"
#include "math/vector3.h"
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
    CharacterController::CharacterController(PhysicsSystem *ps, PassKey<PhysicsSystem>)
        : _transformed_shape{
              ps->create_shape<CapsuleShape>(.5f * height_standing, radius_standing),
              {{0.f, .5f * height_standing + radius_standing, 0.f}, {1.f}, {}}},
          _linear_velocity{}
    {
    }

    auto CharacterController::position() const -> Vector3
    {
        return _transformed_shape.transform().position;
    }

    auto CharacterController::set_position(const Vector3 &pos) -> void
    {
        _transformed_shape.set_position(pos);
    }

    auto CharacterController::bounce(const Vector3 &amount) -> void
    {
        _transformed_shape.translate(amount);
    }

    auto CharacterController::debug_draw(::JPH::DebugRenderer *debug_renderer, PassKey<PhysicsSystem>) const -> void
    {
        return _transformed_shape.shape()->native_handle()->Draw(
            debug_renderer,
            to_jolt(_transformed_shape.transform()),
            ::JPH::Vec3::sOne(),
            ::JPH::Color::sGreen, false, true);
    }

    auto CharacterController::set_linear_velocity(const Vector3 &velocity) -> void
    {
        _linear_velocity = velocity;
    }

    auto CharacterController::update(float delta, PassKey<PhysicsSystem>) -> void
    {
        _transformed_shape.translate(_linear_velocity * delta);
    }

    auto CharacterController::transformed_shape() const -> TransformedShape
    {
        return _transformed_shape;
    }

}
#pragma once

#include <Jolt/Jolt.h>

#include <Jolt/Renderer/DebugRenderer.h>

#include "math/vector3.h"
#include "physics/shape.h"
#include "physics/transformed_shape.h"
#include "utils/auto_release.h"
#include "utils/pass_key.h"

namespace game
{
    class PhysicsSystem;
    class CharacterController
    {
    public:
        CharacterController(PhysicsSystem *ps, PassKey<PhysicsSystem>);
        ~CharacterController() = default;

        auto position() const -> Vector3;
        auto set_position(const Vector3 &pos) -> void;

        auto bounce(const Vector3 &amount) -> void;

        auto debug_draw(::JPH::DebugRenderer *debug_renderer, PassKey<PhysicsSystem>) const -> void;

        auto set_linear_velocity(const Vector3 &velocity) -> void;

        auto update(float delta, PassKey<PhysicsSystem>) -> void;

        auto transformed_shape() const -> TransformedShape;

    private:
        TransformedShape _transformed_shape;
        Vector3 _linear_velocity;
    };
}
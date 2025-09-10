#pragma once

#include <memory>

#include "physics/debug_renderer.h"
#include "physics/rigid_body.h"
#include "physics/shape.h"

namespace game
{
    class PhysicsSystem
    {
    public:
        PhysicsSystem();
        ~PhysicsSystem();

        auto update() -> void;
        auto optimize() const -> void;
        auto debug_renderer() const -> const DebugRenderer &;

        template <class T, class... Args>
        auto create_shape(Args &&...args) const -> T
        {
            return T{std::forward<Args>(args)..., PassKey<PhysicsSystem>{}};
        }

        auto create_rigid_body(const Shape &shape,
                               const Vector3 &position, RigidBodyType type) const -> RigidBody;

    private:
        struct Implementation;
        std::unique_ptr<Implementation> _impl;
    };
}

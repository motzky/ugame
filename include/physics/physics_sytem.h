#pragma once

#include <concepts>
#include <memory>

#include "physics/character_controller.h"
#include "physics/debug_renderer.h"
#include "physics/rigid_body.h"
#include "physics/shape.h"

namespace game
{
    enum class PhysicsLayer
    {
        NON_MOVING,
        MOVING,

        MAX_LAYER
    };

    class Transform;

    template <class T>
    concept IsShape = std::derived_from<T, Shape>;

    class PhysicsSystem
    {
    public:
        PhysicsSystem();
        ~PhysicsSystem();

        auto update() -> void;
        auto optimize() const -> void;
        auto debug_renderer() const -> const DebugRenderer &;

        template <IsShape T, class... Args>
        auto create_shape(Args &&...args) -> T *
        {
            auto &shape = _shapes.emplace_back(std::make_unique<T>(std::forward<Args>(args)..., PassKey<PhysicsSystem>{}));
            return static_cast<T *>(shape.get());
        }

        auto create_rigid_body(const Shape &shape,
                               const Vector3 &position, RigidBodyType type) const -> RigidBody;

        auto character_controller() const -> CharacterController &;

        auto query_collisions(const Shape *shape, const Transform &transform) -> std::vector<const Shape *>;

    private:
        struct Implementation;
        std::unique_ptr<Implementation> _impl;

        std::vector<std::unique_ptr<Shape>> _shapes;
    };
}

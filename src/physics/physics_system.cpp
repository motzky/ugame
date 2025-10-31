#include "physics/physics_sytem.h"

#include <algorithm>
#include <cstdarg>
#include <ranges>
#include <set>
#include <thread>
#include <utility>

#include <Jolt/Jolt.h>

#include <Jolt/Core/Core.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Math/Vec3.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/CollideShape.h>
#include <Jolt/Physics/Collision/CollisionCollector.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/RegisterTypes.h>

#include "log.h"
#include "math/transform.h"
#include "physics/box_shape.h"
#include "physics/character_controller.h"
#include "physics/debug_renderer.h"
#include "physics/jolt_utils.h"
#include "physics/rigid_body.h"
#include "physics/sphere_shape.h"
#include "utils/ensure.h"

using namespace ::JPH::literals;

namespace
{
    auto jolt_trace(const char *fmt, ...) -> void
    {
        va_list list;
        va_start(list, fmt);
        char buffer[1024];
        vsnprintf(buffer, sizeof(buffer), fmt, list);
        va_end(list);

        game::log::info("jolt trace: {}", buffer);
    }

    class SimpleBroadPhaseLayer final : public ::JPH::BroadPhaseLayerInterface
    {
    public:
        virtual auto GetNumBroadPhaseLayers() const -> ::JPH::uint override
        {
            return std::to_underlying(game::PhysicsLayer::MAX_LAYER);
        }

        virtual auto GetBroadPhaseLayer(::JPH::ObjectLayer layer) const -> ::JPH::BroadPhaseLayer override
        {
            return ::JPH::BroadPhaseLayer(static_cast<JPH::BroadPhaseLayer::Type>(layer));
        }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
        virtual auto GetBroadPhaseLayerName(::JPH::BroadPhaseLayer layer) const -> const char * override
        {
            auto native_layer = game::PhysicsLayer{layer.GetValue()};
            switch (native_layer)
            {
                using enum game::PhysicsLayer;
            case NON_MOVING:
                return "NON_MOVING";
            case MOVING:
                return "MOVING";
            default:
                game::ensure(false, "unknown broad phase layer");
                return "INVALID";
            }
        }
#endif
    };

    class SimpleObjectVsBroadPhaseLayerFilter : public ::JPH::ObjectVsBroadPhaseLayerFilter
    {
    public:
        virtual auto ShouldCollide(::JPH::ObjectLayer layer1, ::JPH::BroadPhaseLayer layer2) const -> bool override
        {
            return std::ranges::any_of(
                std::set{game::PhysicsLayer{layer1}, game::PhysicsLayer{layer2.GetValue()}},
                [](const auto &e)
                { return e == game::PhysicsLayer::MOVING; });
        }
    };

    class SimpleObjectLayerPairFilter : public ::JPH::ObjectLayerPairFilter
    {
    public:
        virtual auto ShouldCollide(::JPH::ObjectLayer layer1, ::JPH::ObjectLayer layer2) const -> bool override
        {
            return std::ranges::any_of(
                std::set{game::PhysicsLayer{layer1}, game::PhysicsLayer{layer2}},
                [](const auto &e)
                { return e == game::PhysicsLayer::MOVING; });
        }
    };
}

namespace game
{
    struct PhysicsSystem::Implementation
    {
        SimpleBroadPhaseLayer broad_phase_layer;
        SimpleObjectVsBroadPhaseLayerFilter object_vs_broad_phase_layer;
        SimpleObjectLayerPairFilter object_layer_pair_filter;
        ::JPH::TempAllocatorImpl temp_allocator{10u * 1024u * 1024u};
        ::JPH::JobSystemThreadPool job_system{
            ::JPH::cMaxPhysicsJobs,
            ::JPH::cMaxPhysicsBarriers,
            static_cast<int>(std::thread::hardware_concurrency() - 1u)};
        ::JPH::PhysicsSystem physics_system;
        ::JPH::BodyID sphere_id;
        DebugRenderer debug_renderer = {{}};
        std::unique_ptr<CharacterController> character_controller;
    };

    PhysicsSystem::PhysicsSystem()
        : _impl{},
          _shapes{}
    {
        static bool once = false;

        if (!once)
        {
            once = true;

            ::JPH::RegisterDefaultAllocator();
            ::JPH::Trace = jolt_trace;

            ::JPH::Factory::sInstance = new ::JPH::Factory{};

            ::JPH::RegisterTypes();
        }

        constexpr auto max_bodies = 1024u;
        constexpr auto num_body_mutexes = 0u;
        constexpr auto max_body_pairs = 1024u;
        constexpr auto max_contact_constraints = 1024u;

        _impl = std::make_unique<Implementation>();

        _impl->physics_system.Init(
            max_bodies,
            num_body_mutexes,
            max_body_pairs,
            max_contact_constraints,
            _impl->broad_phase_layer,
            _impl->object_vs_broad_phase_layer,
            _impl->object_layer_pair_filter);

        _impl->physics_system.SetGravity({0.f, -9.80665f, 0.f});

        _impl->character_controller = std::make_unique<CharacterController>(this, PassKey<PhysicsSystem>{});
    }

    auto PhysicsSystem::optimize() const -> void
    {
        _impl->physics_system.OptimizeBroadPhase();
    }

    PhysicsSystem::~PhysicsSystem() = default;

    auto PhysicsSystem::create_rigid_body(const Shape &shape, const Vector3 &position, RigidBodyType type) const -> RigidBody
    {
        auto &body_interface = _impl->physics_system.GetBodyInterface();
        return {shape, position, type, body_interface, {}};
    }

    auto PhysicsSystem::update() -> void
    {
        _impl->physics_system.Update(1.f / 60.f, 1, &_impl->temp_allocator, &_impl->job_system);
        _impl->character_controller->update(1.f / 60.f, {});

        _impl->debug_renderer.clear();
        static const auto settings = ::JPH::BodyManager::DrawSettings{};
        _impl->physics_system.DrawBodies(settings, &_impl->debug_renderer);

        _impl->character_controller->debug_draw(&_impl->debug_renderer, {});
    }

    auto PhysicsSystem::debug_renderer() const -> DebugRenderer &
    {
        return _impl->debug_renderer;
    }

    auto PhysicsSystem::character_controller() const -> CharacterController &
    {
        return *_impl->character_controller;
    }
}
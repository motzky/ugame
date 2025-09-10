#include "physics/physics_sytem.h"

#include <cstdarg>
#include <thread>

#include <Jolt/Jolt.h>

#include <Jolt/Core/Core.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/RegisterTypes.h>

#include "ensure.h"
#include "log.h"
#include "physics/box_shape.h"
#include "physics/debug_renderer.h"
#include "physics/sphere_shape.h"

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
            return 2;
        }

        virtual auto GetBroadPhaseLayer(::JPH::ObjectLayer layer) const -> ::JPH::BroadPhaseLayer override
        {
            return ::JPH::BroadPhaseLayer(static_cast<JPH::BroadPhaseLayer::Type>(layer));
        }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
        virtual auto GetBroadPhaseLayerName(::JPH::BroadPhaseLayer inLayer) const -> const char * override
        {
            switch (inLayer.GetValue())
            {
            case 0:
                return "NON_MOVING";
            case 1:
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
        virtual auto ShouldCollide(::JPH::ObjectLayer inlayer1, ::JPH::BroadPhaseLayer inlayer2) const -> bool override
        {
            switch (inlayer1)
            {
            case 0:
                return inlayer2.GetValue() == 1;
            case 1:
                return true;
            default:
                game::ensure(false, "Not implemented lauer");
                return false;
            }
        }
    };

    class SimpleObjectLayerPairFilter : public ::JPH::ObjectLayerPairFilter
    {
    public:
        virtual auto ShouldCollide(::JPH::ObjectLayer inLayer1, ::JPH::ObjectLayer inLayer2) const -> bool override
        {
            switch (inLayer1)
            {
            case 0:
                return inLayer2 == 1;
            case 1:
                return true;
            default:
                game::ensure(false, "Not implemented leyer");
                return false;
            }
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
    };

    PhysicsSystem::PhysicsSystem()
        : _impl{}
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

        auto &body_interface = (_impl->physics_system).GetBodyInterface();

        auto floor_shape = BoxShape{{50.f, 1.f, 50.f}, {}};
        auto floor_settings = ::JPH::BodyCreationSettings{floor_shape.native_handle(), ::JPH::RVec3{0.0_r, 0.0_r, 0.0_r}, ::JPH::Quat::sIdentity(), ::JPH::EMotionType::Static, 0};

        auto floor = body_interface.CreateBody(floor_settings);
        body_interface.AddBody(floor->GetID(), ::JPH::EActivation::DontActivate);

        auto sphere_shape = SphereShape{5.f, {}};
        auto sphere_settings = ::JPH::BodyCreationSettings{sphere_shape.native_handle(), ::JPH::RVec3{0.0_r, 0.0_r, -10.0_r}, ::JPH::Quat::sIdentity(), ::JPH::EMotionType::Dynamic, 1};

        _impl->sphere_id = body_interface.CreateAndAddBody(sphere_settings, ::JPH::EActivation::Activate);

        body_interface.SetLinearVelocity(_impl->sphere_id, ::JPH::Vec3(0.f, -.01f, 0.f));

        _impl->physics_system.OptimizeBroadPhase();
    }

    PhysicsSystem::~PhysicsSystem() = default;

    auto PhysicsSystem::update() -> void
    {
        _impl->physics_system.Update(1.f / 60.f, 1, &_impl->temp_allocator, &_impl->job_system);

        _impl->debug_renderer.clear();
        static const auto settings = ::JPH::BodyManager::DrawSettings{};
        _impl->physics_system.DrawBodies(settings, &_impl->debug_renderer);
    }

    auto PhysicsSystem::debug_renderer() const -> const DebugRenderer &
    {
        return _impl->debug_renderer;
    }
}
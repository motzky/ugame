#pragma once

#include "game/routines/routine_base.h"
#include "messaging/message_bus.h"
#include "physics/physics_sytem.h"
#include "scheduler/scheduler.h"
#include "scheduler/task.h"

namespace game::routines
{
    class PhysicsRoutine : public RoutineBase
    {
    public:
        PhysicsRoutine(PhysicsSystem &ps, messaging::MessageBus &bus, Scheduler &scheduler);
        ~PhysicsRoutine() override = default;
        PhysicsRoutine(const PhysicsRoutine &) = delete;
        auto operator=(const PhysicsRoutine &) -> PhysicsRoutine & = delete;
        PhysicsRoutine(PhysicsRoutine &&) = default;

        auto create_task() -> Task;
        virtual auto handle_key_press(const KeyEvent &) -> void override;

    private:
        PhysicsSystem &_ps;
        Scheduler &_scheduler;
        bool _show_physics_debug;
        bool _show_debug;
    };
}

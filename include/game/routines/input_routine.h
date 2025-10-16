#pragma once

#include "game/routines/routine_base.h"
#include "messaging/message_bus.h"
#include "scheduler/scheduler.h"
#include "scheduler/task.h"
#include "window.h"

namespace game::routines
{
    class InputRoutine : public RoutineBase
    {
    public:
        InputRoutine(const Window &window, messaging::MessageBus &bus, Scheduler &scheduler);
        ~InputRoutine() override = default;
        InputRoutine(const InputRoutine &) = delete;
        auto operator=(const InputRoutine &) -> InputRoutine & = delete;
        InputRoutine(InputRoutine &&) = default;

        auto create_task() -> Task;

    private:
        const Window &_window;
        Scheduler &_scheduler;
    };
}

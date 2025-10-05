#pragma once

#include "messaging/message_bus.h"
#include "scheduler/scheduler.h"
#include "scheduler/task.h"
#include "window.h"

namespace game::routines
{
    class InputRoutine
    {
    public:
        InputRoutine(const Window &window, messaging::MessageBus &bus, Scheduler &scheduler);
        InputRoutine(const InputRoutine &) = delete;
        auto operator=(const InputRoutine &) -> InputRoutine & = delete;
        InputRoutine(InputRoutine &&) = default;

        auto create_task() -> Task;

    private:
        const Window &_window;
        messaging::MessageBus &_bus;
        Scheduler &_scheduler;
    };
}

#pragma once

#include "game/routines/routine_base.h"
#include "messaging/message_bus.h"
#include "scheduler/scheduler.h"
#include "scheduler/task.h"

namespace game::routines
{
    class SoundRoutine : public RoutineBase
    {
    public:
        SoundRoutine(messaging::MessageBus &bus, Scheduler &scheduler);
        ~SoundRoutine() override = default;
        SoundRoutine(const SoundRoutine &) = delete;
        auto operator=(const SoundRoutine &) -> SoundRoutine & = delete;
        SoundRoutine(SoundRoutine &&) = default;

        auto create_task() -> Task;

    private:
        Scheduler &_scheduler;
    };
}

#include "game/routines/sound_routine.h"

#include <coroutine>

#include "messaging/message_bus.h"
#include "scheduler/scheduler.h"
#include "scheduler/task.h"
#include "scheduler/wait.h"

namespace game::routines
{
    SoundRoutine::SoundRoutine(messaging::MessageBus &bus, Scheduler &scheduler)
        : RoutineBase{bus, {}},
          _scheduler(scheduler)
    {
    }

    auto SoundRoutine::create_task() -> Task
    {
        while (_state != GameState::EXITING)
        {
            if (_state != GameState::EXITING)
            {
                co_await Wait{_scheduler, 1u};
            }
        }
    }

}
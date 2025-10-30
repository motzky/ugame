#ifdef _WIN32
#error This code unit is NOT for Windows !
#endif

#include "game/routines/sound_routine.h"

#include <coroutine>
#include <memory>

#include "messaging/message_bus.h"
#include "scheduler/scheduler.h"
#include "scheduler/task.h"
#include "scheduler/wait.h"

namespace game::routines
{
    struct SoundRoutine::implementation
    {
    };

    SoundRoutine::SoundRoutine(messaging::MessageBus &bus, Scheduler &scheduler, DefaultCache &resource_cache)
        : RoutineBase{bus, {}},
          _impl(std::make_unique<SoundRoutine::implementation>()),
          _scheduler(scheduler),
          _resource_cache(resource_cache)
    {
    }

    SoundRoutine::~SoundRoutine() = default;

    auto SoundRoutine::create_task() -> Task
    {
        while (_state != GameState::EXITING)
        {
            if (_state != GameState::EXITING)
            {
                co_await Wait{_scheduler, 1u};
            }
        }
        log::info("SoundRoutine ending");
    }

}
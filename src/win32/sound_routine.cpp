#ifndef _WIN32
#error This code unit is ONLY for Windows !
#endif

#include "game/routines/sound_routine.h"

#include <coroutine>
#include <memory>

#include "messaging/message_bus.h"
#include "scheduler/scheduler.h"
#include "scheduler/task.h"
#include "scheduler/wait.h"
#include "utils/ensure.h"

#include <xaudio2.h>

namespace
{
    const auto release = [](auto *obj)
    { obj->Release(); };
}

namespace game::routines
{
    struct SoundRoutine::implementation
    {
        std::unique_ptr<::IXAudio2, decltype(release)> xaudio;
    };

    SoundRoutine::SoundRoutine(messaging::MessageBus &bus, Scheduler &scheduler)
        : RoutineBase{bus, {}},
          _impl(std::make_unique<SoundRoutine::implementation>()),
          _scheduler(scheduler)
    {
        ensure(::XAudio2Create(std::out_ptr(_impl->xaudio), 0, XAUDIO2_DEFAULT_PROCESSOR) == S_OK, "failed to create xaudio2");
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
    }

}
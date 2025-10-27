#ifndef _WIN32
#error This code unit is ONLY for Windows !
#endif

#include "game/routines/sound_routine.h"

#include <algorithm>
#include <coroutine>
#include <cstddef>
#include <memory>
#include <ranges>
#include <span>
#include <utility>
#include <vector>

#include "file.h"
#include "messaging/message_bus.h"
#include "scheduler/scheduler.h"
#include "scheduler/task.h"
#include "scheduler/wait.h"
#include "utils/ensure.h"

#include <xaudio2.h>

namespace
{
    constexpr auto release = [](auto *obj)
    { obj->Release(); };
    constexpr auto destroy_voice = [](auto *obj)
    { obj->DestroyVoice(); };
}

namespace game::routines
{
    struct SoundRoutine::implementation
    {
        std::unique_ptr<::IXAudio2, decltype(release)> xaudio;
        std::unique_ptr<::IXAudio2MasteringVoice, decltype(destroy_voice)> mastering_voice;
        std::unique_ptr<::IXAudio2SourceVoice, decltype(destroy_voice)> source_voice;
        std::vector<std::byte> sound_data;
    };

    SoundRoutine::SoundRoutine(messaging::MessageBus &bus, Scheduler &scheduler, DefaultCache &resource_cache)
        : RoutineBase{bus, {}},
          _impl(std::make_unique<SoundRoutine::implementation>()),
          _scheduler(scheduler),
          _resource_cache(resource_cache)
    {
        ensure(::XAudio2Create(std::out_ptr(_impl->xaudio), 0, XAUDIO2_DEFAULT_PROCESSOR) == S_OK, "failed to create xaudio2");
        ensure(_impl->xaudio->CreateMasteringVoice(
                   std::out_ptr(_impl->mastering_voice),
                   XAUDIO2_DEFAULT_CHANNELS,
                   XAUDIO2_DEFAULT_SAMPLERATE,
                   0u, nullptr, nullptr) == S_OK,
               "failed to create mastering voice");

        if (!_resource_cache.contains<SoundData>("main_theme"))
        {
            log::error("Could not find sound data in cache: {}", "main_theme");
        }
        else
        {
            const *auto main_theme_data = _resource_cache.get<SoundData>("main_theme");

            auto wfx = ::WAVEFORMATEXTENSIBLE{};
            ensure(main_theme_data.format.size_bytes() >= sizeof(wfx), "fmt chunk to small");
            std::memcpy(&wfx, main_theme_data.format..data(), sizeof(wfx));

            _impl->sound_data = main_theme_data.data | std::ranges::to<std::vector>();

            const auto xaudio_buffer = ::XAUDIO2_BUFFER{
                .Flags = XAUDIO2_END_OF_STREAM,
                .AudioBytes = static_cast<::UINT32>(_impl->sound_data.size_bytes()),
                .pAudioData = reinterpret_cast<const ::BYTE *>(_impl->sound_data.data()),
                .LoopCount = XAUDIO2_LOOP_INFINITE};

            ensure(_impl->xaudio->CreateSourceVoice(std::out_ptr(_impl->source_voice), &wfx.Format) == S_OK, "failed to create source voice");

            ensure(_impl->source_voice->SubmitSourceBuffer(&xaudio_buffer) == S_OK, "failed to submit source voice buffer");
            ensure(_impl->source_voice->Start(0) == S_OK, "failed to start source voice");
        }
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

        ensure(_impl->source_voice->Stop(0) == S_OK, "failed to stop sound");
    }

}
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
    const std::byte riff_cc[] = {std::byte{'R'}, std::byte{'I'}, std::byte{'F'}, std::byte{'F'}};
    const std::byte wave_cc[] = {std::byte{'W'}, std::byte{'A'}, std::byte{'V'}, std::byte{'E'}};
    const std::byte fmt_cc[] = {std::byte{'f'}, std::byte{'m'}, std::byte{'t'}, std::byte{' '}};
    const std::byte data_cc[] = {std::byte{'d'}, std::byte{'a'}, std::byte{'t'}, std::byte{'a'}};

    constexpr auto release = [](auto *obj)
    { obj->Release(); };
    constexpr auto destroy_voice = [](auto *obj)
    { obj->DestroyVoice(); };

    auto find_chunk(std::span<const std::byte> source, std::span<const std::byte> control_code) -> std::span<const std::byte>
    {
        const auto chunk_header = std::ranges::search(source, control_code);
        game::ensure(!chunk_header.empty(), "could not find control code in chunk");

        const auto header_offset = std::ranges::distance(std::ranges::cbegin(data), std::ranges::cbegin(chunk_header));

        auto chunk_size = std::uint32_t{};
        std::memcpy(&chunk_size, source.data() + header_offset + chunk_header.size(), sizeof(chunk_size));

        return std::span{source.data() + header_offset + chunk_header.size() + sizeof(chunk_size), chunk_size};
    }

    auto parse_chunk(std::span<const std::byte> source) -> std::span<const std::byte>
    {
        const auto chunk_header = std::ranges::search(source, riff_cc);
        if (!chunk_header.empty())
        {
            return std::span{source.data() + chunk_header.size_bytes() + 4u, 4u};
        }

        auto chunk_size = std::uint32_t{};
        std::memcpy(&chunk_size, source.data() + 4u, sizeof(chunk_size));

        if (chunk_size % 2 != 0)
        {
            // data is padded to WORD boundary, but chunk_size field does only count data, not padding
            ++chunk_size;
        }

        return std::span{source.data() + 8u, chunk_size};
    }
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

    SoundRoutine::SoundRoutine(messaging::MessageBus &bus, Scheduler &scheduler)
        : RoutineBase{bus, {}},
          _impl(std::make_unique<SoundRoutine::implementation>()),
          _scheduler(scheduler)
    {
        ensure(::XAudio2Create(std::out_ptr(_impl->xaudio), 0, XAUDIO2_DEFAULT_PROCESSOR) == S_OK, "failed to create xaudio2");
        ensure(_impl->xaudio->CreateMasteringVoice(
                   std::out_ptr(_impl->mastering_voice),
                   XAUDIO2_DEFAULT_CHANNELS,
                   XAUDIO2_DEFAULT_SAMPLERATE,
                   0u, nullptr, nullptr) == S_OK,
               "failed to create mastering voice");

        const auto audio_file = File{"./assets/main_theme.wav"};
        const auto data = audio_file.as_bytes();

        const auto riff_chunk = find_chunk(data, riff_cc);
        const auto filetype = parse_chunk(riff_chunk);
        ensure(std::ranges::equals(filetype, wave_cc), "Wrong file type: {}", filetype);

        const auto fmt_chunk = find_chunk(wave_chunk, fmt_cc);
        const auto fmt_data = parse_chunk(fmt_chunk);

        auto wfx = ::WAVEFORMATEXTENSIBLE{};
        ensure(fmt_data.size_bytes() >= sizeof(wfx), "fmt chunk to small");
        std::memcpy(&wfx, fmt_data.data(), sizeof(wfx));

        const auto data_chunk = find_chunk(wave_chunk, data_cc);
        const auto data_chunk_data = parse_chunk(data_chunk);

        _impl->sound_data = data_chunk_data | std::ranges::to<std::vector>();

        const auto xaudio_buffer = ::XAUDIO2_BUFFER{
            .Flags = XAUDIO2_END_OF_STREAM,
            .AudioBytes = static_cast<::UINT32>(_impl->sound_data.size_bytes()),
            .pAudioData = reinterpret_cast<const ::BYTE *>(_impl->sound_data.data())};

        ensure(_impl->xaudio->CreateSourceVoice(std::out_ptr(_impl->source_voice), &wfx.Format) == S_OK, "failed to create source voice");

        ensure(_impl->source_voice->SubmitSourceBuffer(&xaudio_buffer) == S_OK, "failed to submit source voice buffer");
        ensure(_impl->source_voice->Start(0) == S_OK, "failed to start source voice");
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
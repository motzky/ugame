#pragma once

#include <coroutine>
#include <cstdint>
#include <deque>
#include <optional>

#include "scheduler/task.h"

namespace game
{
    class Scheduler
    {
    public:
        Scheduler();
        auto add(Task task) -> void;
        auto reschedule(std::coroutine_handle<> handle, std::uint32_t wait_tick) -> void;

        auto run() -> void;

    private:
        struct WaitTask
        {
            Task task;
            std::optional<std::size_t> tick_target;
        };

        std::deque<WaitTask> _queue;
        std::size_t _tick_count;
    };
}
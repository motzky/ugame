#pragma once

#include <chrono>
#include <coroutine>
#include <cstdint>
#include <functional>
#include <optional>
#include <variant>
#include <vector>

#include "scheduler/task.h"

namespace game
{
    class Scheduler
    {
    public:
        Scheduler();
        auto add(Task task) -> void;
        auto reschedule(std::coroutine_handle<> handle, std::size_t wait_tick) -> void;
        auto reschedule(std::coroutine_handle<> handle, std::chrono::nanoseconds wait_tick) -> void;

        auto run() -> void;

    private:
        struct WaitTask
        {
            Task task;
            std::function<bool()> check_resume;
        };

        std::vector<WaitTask> _queue;
        std::size_t _tick_count;
        std::chrono::nanoseconds _elapsed;
    };
}
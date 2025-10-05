#pragma once

#include <chrono>
#include <coroutine>
#include <cstdint>
#include <deque>
#include <functional>
#include <memory>

#include "scheduler/task.h"

namespace game
{
    class Scheduler
    {
    public:
        Scheduler();
        auto add(Task task) -> void;
        auto add(Task task, std::uint32_t *wait_count) -> void;

        auto reschedule(std::coroutine_handle<> handle, std::size_t wait_tick) -> void;
        auto reschedule(std::coroutine_handle<> handle, std::chrono::nanoseconds wait_tick) -> void;
        auto reschedule(std::coroutine_handle<> handle, std::unique_ptr<std::uint32_t> counter) -> void;

        auto run() -> void;

    private:
        struct WaitTask
        {
            Task task;
            std::move_only_function<bool()> check_resume;
            std::uint32_t *parent_wait_count;
        };

        std::deque<WaitTask> _queue;
        std::size_t _tick_count;
        std::chrono::nanoseconds _elapsed;
    };
}
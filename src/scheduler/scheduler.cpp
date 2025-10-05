#include "scheduler/scheduler.h"

#include <algorithm>
#include <deque>
#include <optional>
#include <ranges>

#include "log.h"
#include "scheduler/task.h"
#include "utils/ensure.h"

namespace game
{
    Scheduler::Scheduler()
        : _queue{},
          _tick_count{},
          _elapsed{}
    {
    }
    auto Scheduler::add(Task task) -> void
    {
        _queue.push_back({std::move(task), nullptr});
    }

    auto Scheduler::reschedule(std::coroutine_handle<> handle, std::size_t wait_ticks) -> void
    {
        auto task = std::ranges::find_if(_queue, [handle](const auto &e)
                                         { return e.task.has_handle(handle); });
        expect(task != std::ranges::end(_queue), "could not find task");

        const auto future_tick = wait_ticks + _tick_count;
        task->check_resume = [future_tick, this]
        { return _tick_count == future_tick; };
    }

    auto Scheduler::reschedule(std::coroutine_handle<> handle, std::chrono::nanoseconds wait_time) -> void
    {
        auto task = std::ranges::find_if(_queue, [handle](const auto &e)
                                         { return e.task.has_handle(handle); });
        expect(task != std::ranges::end(_queue), "could not find task");

        const auto future_time = wait_time + _elapsed;
        task->check_resume = [future_time, this]
        { return _elapsed >= future_time; };
    }

    auto Scheduler::run() -> void
    {
        while (!_queue.empty())
        {
            const auto start = std::chrono::steady_clock::now();
            for (auto &[task, check_resume] : _queue)
            {
                expect(task.can_resume(), "bad task in queue");

                if (!check_resume || check_resume())
                {
                    task.resume();
                }
            }

            std::erase_if(_queue,
                          [](const auto &e)
                          { return !e.task.can_resume(); });

            ++_tick_count;
            _elapsed += std::chrono::steady_clock::now() - start;
        }
    }

}
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
        _queue.push_front({std::move(task), std::nullopt});
    }

    auto Scheduler::reschedule(std::coroutine_handle<> handle, std::size_t wait_ticks) -> void
    {
        auto task = std::ranges::find_if(_queue, [handle](const auto &e)
                                         { return e.task.has_handle(handle); });
        expect(task != std::ranges::end(_queue), "could not find task");

        task->target = wait_ticks + _tick_count;
    }

    auto Scheduler::reschedule(std::coroutine_handle<> handle, std::chrono::nanoseconds wait_time) -> void
    {
        auto task = std::ranges::find_if(_queue, [handle](const auto &e)
                                         { return e.task.has_handle(handle); });
        expect(task != std::ranges::end(_queue), "could not find task");

        task->target = wait_time + _elapsed;
    }

    auto Scheduler::run() -> void
    {
        while (!_queue.empty())
        {
            const auto start = std::chrono::steady_clock::now();
            for (auto &[task, tick_target] : _queue)
            {
                expect(task.can_resume(), "bad task in queue");

                if (tick_target)
                {
                    if (const auto *v = std::get_if<std::size_t>(&*tick_target); v)
                    {
                        expect(*v >= _tick_count, "invalid tick target");
                        if (*v == _tick_count)
                        {
                            task.resume();
                        }
                    }
                    else if (const auto *v = std::get_if<std::chrono::nanoseconds>(&*tick_target); v)
                    {
                        if (_elapsed >= *v)
                        {
                            task.resume();
                        }
                    }
                }
                else
                {
                    task.resume();
                }
            }

            _queue.erase(std::remove_if(std::begin(_queue),
                                        std::end(_queue),
                                        [](const auto &e)
                                        { return !e.task.can_resume(); }),
                         std::ranges::end(_queue));

            ++_tick_count;
            _elapsed += std::chrono::steady_clock::now() - start;
        }
    }

}
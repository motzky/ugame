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
          _tick_count{}
    {
    }
    auto Scheduler::add(Task task) -> void
    {
        _queue.push_front({std::move(task), std::nullopt});
    }

    auto Scheduler::reschedule(std::coroutine_handle<> handle, std::uint32_t wait_ticks) -> void
    {
        auto task = std::ranges::find_if(_queue, [handle](const auto &e)
                                         { return e.task.has_handle(handle); });
        expect(task != std::ranges::end(_queue), "could not find task");

        task->tick_target = wait_ticks + _tick_count;
    }

    auto Scheduler::run() -> void
    {
        while (!_queue.empty())
        {
            for (auto &[task, tick_target] : _queue)
            {
                expect(task.can_resume(), "bad task in queue");

                if (tick_target)
                {
                    expect(*tick_target >= _tick_count, "invalid tick target");
                    if (*tick_target == _tick_count)
                    {
                        task.resume();
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
        }
    }

}
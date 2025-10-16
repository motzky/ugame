#include "scheduler/scheduler.h"

#include <algorithm>
#include <deque>
#include <optional>
#include <ranges>

#include "game/game_state.h"
#include "log.h"
#include "messaging/message_bus.h"
#include "scheduler/task.h"
#include "utils/ensure.h"

namespace game
{
    Scheduler::Scheduler(messaging::MessageBus &bus)
        : _auto_subscribe{bus, {messaging::MessageType::STATE_CHANGE}, this},
          _queue{},
          _tick_count{},
          _elapsed{},
          _state{GameState::RUNNING},
          _next_state{GameState::RUNNING}
    {
    }
    auto Scheduler::add(Task task) -> void
    {
        _queue.push_back({std::move(task), nullptr, nullptr});
    }

    auto Scheduler::add(Task task, std::uint32_t *wait_count) -> void
    {
        _queue.push_back({std::move(task), nullptr, wait_count});
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

    auto Scheduler::reschedule(std::coroutine_handle<> handle, std::unique_ptr<std::uint32_t> counter) -> void
    {
        auto task = std::ranges::find_if(_queue, [handle](const auto &e)
                                         { return e.task.has_handle(handle); });
        expect(task != std::ranges::end(_queue), "could not find task");

        task->check_resume = [counter = std::move(counter)]()
        {
            return *counter == 0;
        };
    }

    auto Scheduler::reschedule(std::coroutine_handle<> handle, GameState state) -> void
    {
        auto task = std::ranges::find_if(_queue, [handle](const auto &e)
                                         { return e.task.has_handle(handle); });
        expect(task != std::ranges::end(_queue), "could not find task");

        task->check_resume = [state, this]
        { return _state == state || _state == GameState::EXITING; };
    }

    auto Scheduler::run() -> void
    {
        while (!_queue.empty())
        {
            const auto start = std::chrono::steady_clock::now();
            for (auto &[task, check_resume, parent_counter] : _queue)
            {
                expect(task.can_resume(), "bad task in queue");

                if (!check_resume || check_resume())
                {
                    task.resume();
                    if (parent_counter)
                    {
                        --(*parent_counter);
                    }
                }
            }
            std::erase_if(_queue,
                          [](const auto &e)
                          { return !e.task.can_resume(); });

            ++_tick_count;
            _elapsed += std::chrono::steady_clock::now() - start;
            _state = _next_state;
        }
    }

    auto Scheduler::handle_state_change(GameState state) -> void
    {
        _next_state = state;
    }
}
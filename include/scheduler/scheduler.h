#pragma once

#include <chrono>
#include <coroutine>
#include <cstdint>
#include <deque>
#include <functional>
#include <memory>

#include "game/game_state.h"
#include "messaging/auto_subscribe.h"
#include "messaging/message_bus.h"
#include "messaging/subscriber.h"
#include "scheduler/task.h"

namespace game
{
    class Scheduler : public messaging::Subscriber
    {
    public:
        Scheduler(messaging::MessageBus &bus);
        ~Scheduler() override = default;
        auto add(Task task) -> void;
        auto add(Task task, std::uint32_t *wait_count) -> void;

        auto reschedule(std::coroutine_handle<> handle, std::size_t wait_tick) -> void;
        auto reschedule(std::coroutine_handle<> handle, std::chrono::nanoseconds wait_tick) -> void;
        auto reschedule(std::coroutine_handle<> handle, std::unique_ptr<std::uint32_t> counter) -> void;
        auto reschedule(std::coroutine_handle<> handle, GameState state) -> void;

        auto run() -> void;

        auto handle_state_change(GameState state) -> void override;

    private:
        struct WaitTask
        {
            Task task;
            std::move_only_function<bool()> check_resume;
            std::uint32_t *parent_wait_count;
        };

        messaging::AutoSubscribe _auto_subscribe;
        std::deque<WaitTask> _queue;
        std::size_t _tick_count;
        std::chrono::nanoseconds _elapsed;
        GameState _state;
        GameState _next_state;
    };
}
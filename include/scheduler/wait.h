#pragma once

#include <chrono>
#include <coroutine>
#include <cstdint>
#include <variant>

#include "scheduler/scheduler.h"

namespace game
{
    struct Wait
    {
        Wait(Scheduler &scheduler, std::uint32_t ticks)
            : wait_object{ticks},
              scheduler(scheduler)
        {
        }

        Wait(Scheduler &scheduler, std::chrono::nanoseconds wait_time)
            : wait_object{wait_time},
              scheduler(scheduler)
        {
        }

        auto await_ready() -> bool
        {
            return false;
        }

        auto await_suspend(std::coroutine_handle<> h) -> void
        {
            if (const auto *v = std::get_if<std::size_t>(&wait_object); v)
            {
                scheduler.reschedule(h, *v);
            }
            else if (const auto *v = std::get_if<std::chrono::nanoseconds>(&wait_object); v)
            {
                scheduler.reschedule(h, *v);
            }
        }

        auto await_resume()
        {
        }

        std::variant<std::size_t, std::chrono::nanoseconds> wait_object;
        Scheduler &scheduler;
    };
}
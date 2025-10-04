#pragma once

#include <coroutine>
#include <cstdint>

#include "scheduler/scheduler.h"

namespace game
{
    struct Wait
    {
        explicit Wait(Scheduler &scheduler, std::uint32_t ticks)
            : wait_ticks(ticks),
              scheduler(scheduler)
        {
        }

        auto await_ready() -> bool
        {
            return false;
        }

        auto await_suspend(std::coroutine_handle<> h) -> void
        {
            scheduler.reschedule(h, wait_ticks);
        }

        auto await_resume()
        {
        }

        std::uint32_t wait_ticks;
        Scheduler &scheduler;
    };
}
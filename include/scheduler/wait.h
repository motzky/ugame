#pragma once

#include <coroutine>

#include "scheduler/scheduler.h"

namespace game
{
    template <class T>
    class Wait
    {
    public:
        Wait(Scheduler &scheduler, T wait_object)
            : _wait_object{std::move(wait_object)},
              _scheduler(scheduler)
        {
        }

        auto await_ready() -> bool
        {
            return false;
        }

        auto await_suspend(std::coroutine_handle<> h) -> void
        {
            _scheduler.reschedule(h, _wait_object);
        }

        auto await_resume()
        {
        }

    private:
        T _wait_object;
        Scheduler &_scheduler;
    };
}
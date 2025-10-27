#include "game/routines/physics_routine.h"

#include <chrono>
#include <coroutine>

#include "messaging/message_bus.h"
#include "physics/physics_sytem.h"
#include "scheduler/scheduler.h"
#include "scheduler/task.h"
#include "scheduler/wait.h"

using namespace std::literals;

namespace game::routines
{
    PhysicsRoutine::PhysicsRoutine(PhysicsSystem &ps, messaging::MessageBus &bus, Scheduler &scheduler)
        : RoutineBase{bus, {messaging::MessageType::KEY_PRESS}},
          _ps(ps),
          _scheduler(scheduler)
    {
    }

    auto PhysicsRoutine::create_task() -> Task
    {
        while (_state != GameState::EXITING)
        {
            if (_state != GameState::RUNNING)
            {
                co_await Wait{_scheduler, GameState::RUNNING};
            }

            _ps.update();

            if (_state != GameState::EXITING)
            {
                co_await Wait{_scheduler, 16666us};
            }
        }
    }

    auto PhysicsRoutine::handle_key_press(const KeyEvent &event) -> void
    {
        if (event.key() == Key::F2 && event.state() == KeyState::UP)
        {
            _show_physics_debug = !_show_physics_debug;
        }
    }

}
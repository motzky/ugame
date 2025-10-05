#include "game/routines/input_routine.h"

#include <coroutine>

#include "messaging/message_bus.h"
#include "scheduler/scheduler.h"
#include "scheduler/task.h"
#include "scheduler/wait.h"
#include "window.h"

namespace game::routines
{
    InputRoutine::InputRoutine(const Window &window, messaging::MessageBus &bus, Scheduler &scheduler)
        : _window(window),
          _bus(bus),
          _scheduler(scheduler)
    {
    }

    auto InputRoutine::create_task() -> Task
    {
        auto running = true;
        auto show_debug = false;
        while (running)
        {
            auto event = _window.pump_event();
            while (event && running)
            {
                std::visit(
                    [&](auto &&arg)
                    {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr (std::same_as<T, game::StopEvent>)
                        {
                            running = false;
                        }
                        else if constexpr (std::same_as<T, game::KeyEvent>)
                        {
                            // game::log::debug("{}", arg);

                            _bus.post_key_press(arg);

                            if (arg.key() == game::Key::F1 && arg.state() == game::KeyState::UP)
                            {
                                show_debug = !show_debug;
                                _window.show_cursor(show_debug);
                            }
                            else if (arg.key() == game::Key::F2 && arg.state() == game::KeyState::UP)
                            {
                            }
                        }
                        else if constexpr (std::same_as<T, game::MouseEvent>)
                        {
                            if (!show_debug)
                            {
                                _bus.post_mouse_move(arg);
                            }
                        }
                        else if constexpr (std::same_as<T, game::MouseButtonEvent>)
                        {
                            _bus.post_mouse_button(arg);
                            // debug_ui.add_mouse_event(arg);
                        }
                    },
                    *event);
                event = _window.pump_event();
            }

            if (running)
            {
                co_await Wait{_scheduler, 1u};
            }
            else
            {
                _bus.post_quit();
            }
        }
    }

}
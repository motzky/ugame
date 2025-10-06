#include "game/routines/render_routine.h"

#include <coroutine>

#include "game/routines/level_routine.h"
#include "graphics/debug_ui.h"
#include "graphics/renderer.h"
#include "graphics/shape_wireframe_renderer.h"
#include "loaders/mesh_loader.h"
#include "messaging/message_bus.h"
#include "scheduler/scheduler.h"
#include "scheduler/task.h"
#include "scheduler/wait.h"
#include "tlv/tlv_reader.h"
#include "window.h"

namespace game::routines
{
    RenderRoutine::RenderRoutine(
        const LevelRoutine &level_routine,
        const Window &window,
        messaging::MessageBus &bus,
        Scheduler &scheduler,
        const TlvReader &reader,
        MeshLoader &mesh_loader)
        : _level_routine(level_routine),
          _window(window),
          _bus(bus),
          _scheduler(scheduler),
          _renderer{reader,
                    mesh_loader,
                    _window.width(),
                    _window.height()},
          _debug_wireframe_renderer{},
          _show_physics_debug(false),
          _show_debug(false),
          _running(true),
          _auto_subscribe{bus, {messaging::MessageType::KEY_PRESS, messaging::MessageType::MOUSE_MOVE, messaging::MessageType::QUIT}, this}
    {
    }

    auto RenderRoutine::create_task() -> Task
    {
        auto gamma = 2.2f;
        // const auto debug_ui = game::DebugUi(_window.native_handle(), _level_routine.level().scene(), _player.camera(), gamma);

        while (_running)
        {
            auto level = _level_routine.level();

            if (_show_physics_debug)
            {
                auto lines = _debug_wireframe_renderer.yield();
                for (const auto line : level->physics().debug_renderer().lines())
                {
                    lines.push_back(line);
                }
                level->scene().debug_lines = game::DebugLines{lines};

                level->physics().debug_renderer().clear();
            }
            else
            {
                level->scene().debug_lines.reset();
            }

            _renderer.render(_level_routine.player().camera(), _level_routine.level()->scene(), gamma);
            if (_show_debug)
            {
                // _debug_ui.render();
            }

            _window.swap();

            if (_running)
            {
                co_await Wait{_scheduler, 1u};
            }
        }
    }

    auto RenderRoutine::handle_key_press(const KeyEvent &event) -> void
    {
        if (event.key() == game::Key::F1 && event.state() == game::KeyState::UP)
        {
            _show_debug = !_show_debug;
        }
        else if (event.key() == game::Key::F2 && event.state() == game::KeyState::UP)
        {
            _show_physics_debug = !_show_physics_debug;
        }
    }

    auto RenderRoutine::handle_quit() -> void
    {
        _running = false;
    }

    auto RenderRoutine::handle_mouse_move([[maybe_unused]] const MouseEvent &event) -> void
    {
        // _debug_ui.add_mouse_event(event);
    }

}
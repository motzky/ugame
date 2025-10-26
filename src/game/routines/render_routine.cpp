#include "game/routines/render_routine.h"

#include <coroutine>

#include "game/routines/level_routine.h"
#include "game/routines/routine_base.h"
#include "graphics/debug_ui.h"
#include "graphics/renderer.h"
#include "graphics/shape_wireframe_renderer.h"
#include "loaders/mesh_loader.h"
#include "messaging/message_bus.h"
#include "scheduler/scheduler.h"
#include "scheduler/task.h"
#include "scheduler/wait.h"
#include "tlv/tlv_reader.h"
#include "utils/ensure.h"
#include "window.h"

namespace game::routines
{
    RenderRoutine::RenderRoutine(
        const Window &window,
        messaging::MessageBus &bus,
        Scheduler &scheduler,
        const TlvReader &reader,
        MeshLoader &mesh_loader,
        std::uint8_t samples)
        : RoutineBase(bus, {messaging::MessageType::KEY_PRESS,
                            messaging::MessageType::MOUSE_MOVE,
                            messaging::MessageType::CHANGE_CAMERA,
                            messaging::MessageType::CHANGE_SCENE}),
          _window(window),
          _scheduler(scheduler),
          _renderer{reader,
                    mesh_loader,
                    _window.width(),
                    _window.height(),
                    samples},
          _debug_wireframe_renderer{},
          _scene{},
          _camera{},
          _show_physics_debug(false),
          _show_debug(false)
    {
    }

    auto RenderRoutine::create_task() -> Task
    {
        auto gamma = 2.2f;
        // const auto debug_ui = game::DebugUi(_window.native_handle(), _level_routine.level().scene(), _player.camera(), gamma);

        while (_state != GameState::EXITING)
        {
            expect(_scene, "scene cannot be null");
            expect(_camera, "camera cannot be null");

            _renderer.render(*_camera, *_scene, gamma);
            if (_show_debug)
            {
                // _debug_ui.render();
            }

            _window.swap();

            if (_state != GameState::EXITING)
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

    auto RenderRoutine::handle_mouse_move([[maybe_unused]] const MouseEvent &event) -> void
    {
        // _debug_ui.add_mouse_event(event);
    }

    auto RenderRoutine::handle_change_camera(const Camera *camera) -> void
    {
        _camera = camera;
    }
    auto RenderRoutine::handle_change_scene(Scene *scene) -> void
    {
        _scene = scene;
    }

}
#include "game/routines/main_menu_routine.h"

#include <coroutine>
#include <numbers>
#include <string>

#include "game/routines/routine_base.h"
#include "graphics/camera.h"
#include "graphics/scene.h"
#include "log.h"
#include "messaging/message_bus.h"
#include "messaging/subscriber.h"
#include "resources/resource_cache.h"
#include "scheduler/scheduler.h"
#include "scheduler/task.h"
#include "scheduler/wait.h"
#include "tlv/tlv_reader.h"

namespace
{
    auto create_camera(const game::Window &window) -> game::Camera
    {
        return {{0.f, 2.f, 20.f},
                {0.f, 0.f, 0.f},
                {0.f, 1.f, 0.f},
                std::numbers::pi_v<float> / 4.f,
                static_cast<float>(window.width()),
                static_cast<float>(window.height()),
                0.1f,
                500.f};
    }
}

namespace game::routines
{
    MainMenuRoutine::MainMenuRoutine(const Window &window, messaging::MessageBus &bus, Scheduler &scheduler, DefaultCache &resource_cache, const TlvReader &reader,
                                     const ResourceLoader &resource_loader)
        : RoutineBase{bus, {messaging::MessageType::KEY_PRESS}},
          _window{window},
          _scheduler{scheduler},
          _resource_cache{resource_cache},
          _resource_loader{resource_loader},
          _reader{reader},
          _camera(create_camera(window)),
          _scene{}
    {
        _window.set_title("game: MainMenu");

        _scene = Scene{
            .entities = {},
            .ambient = {.r = .5f, .g = .5f, .b = .5f},
            .directional = {},
            .points = {},
            .debug_lines = {},
            .skybox = nullptr,
            .skybox_sampler = _resource_cache.get<TextureSampler>("sky_box"),
            .labels = {},
            .effects = {.hdr = true, .grey_scale = false, .blur = false}};

        _bus.post_change_camera(&_camera);
        _bus.post_change_scene(&_scene);
    }

    auto MainMenuRoutine::create_task() -> Task
    {
        while (_state != GameState::EXITING)
        {
            if (_state != GameState::MAIN_MENU)
            {
                co_await Wait{_scheduler, GameState::MAIN_MENU};
            }

            if (_state != GameState::EXITING)
            {
                co_await Wait{_scheduler, 16u};
            }
        }
    }

    auto MainMenuRoutine::handle_key_press(const KeyEvent &event) -> void
    {
        if (_state == GameState::MAIN_MENU)
        {
            if (event.key() == Key::ESC && event.state() == KeyState::UP)
            {
                _bus.post_state_change(GameState::EXITING);
                return;
            }
            if (event.state() == KeyState::UP)
            {
                _bus.post_state_change(GameState::RUNNING);
            }
        }
    }
}
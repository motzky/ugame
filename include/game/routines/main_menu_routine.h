#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "game/routines/routine_base.h"
#include "graphics/camera.h"
#include "graphics/scene.h"
#include "graphics/texture.h"
#include "messaging/auto_subscribe.h"
#include "messaging/message_bus.h"
#include "messaging/subscriber.h"
#include "primitives/entity.h"
#include "resources/resource_cache.h"
#include "resources/resource_loader.h"
#include "scheduler/scheduler.h"
#include "scheduler/task.h"
#include "tlv/tlv_reader.h"
#include "window.h"

namespace game::routines
{
    class MainMenuRoutine : public RoutineBase
    {
    public:
        MainMenuRoutine(const Window &window, messaging::MessageBus &bus, Scheduler &scheduler, DefaultCache &resource_cache, const TlvReader &reader, const ResourceLoader &resource_loader);
        ~MainMenuRoutine() override = default;
        MainMenuRoutine(const MainMenuRoutine &) = delete;
        auto operator=(const MainMenuRoutine &) -> MainMenuRoutine & = delete;
        MainMenuRoutine(MainMenuRoutine &&) = default;

        auto create_task() -> Task;

        virtual auto handle_key_press(const KeyEvent &) -> void override;

    private:
        const Window &_window;
        Scheduler &_scheduler;
        DefaultCache &_resource_cache;
        std::vector<Entity> _entities;
        std::vector<Entity> _level_entities;
        std::vector<Texture> _labels;
        const ResourceLoader &_resource_loader;
        const TlvReader &_reader;
        Camera _camera;
        Scene _scene;
    };
}

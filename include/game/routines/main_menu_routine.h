#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "game/player.h"
#include "game/routines/routine_base.h"
#include "messaging/auto_subscribe.h"
#include "messaging/message_bus.h"
#include "messaging/subscriber.h"
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
        auto player() const -> const Player &;

        virtual auto handle_key_press(const KeyEvent &) -> void override;

    private:
        const Window &_window;
        Scheduler &_scheduler;
        Player _player;
        DefaultCache &_resource_cache;
        const ResourceLoader &_resource_loader;
        const TlvReader &_reader;
    };
}

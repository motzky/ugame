#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "game/levels/lua_level.h"
#include "game/player.h"
#include "messaging/auto_subscribe.h"
#include "messaging/message_bus.h"
#include "messaging/subscriber.h"
#include "resources/resource_cache.h"
#include "scheduler/scheduler.h"
#include "scheduler/task.h"
#include "scripting/script_loader.h"
#include "tlv/tlv_reader.h"
#include "window.h"

namespace game::routines
{
    class LevelRoutine : public messaging::Subscriber
    {
    public:
        LevelRoutine(const Window &window, messaging::MessageBus &bus, Scheduler &scheduler, DefaultCache &resource_cache, const TlvReader &reader);
        LevelRoutine(const LevelRoutine &) = delete;
        auto operator=(const LevelRoutine &) -> LevelRoutine & = delete;
        LevelRoutine(LevelRoutine &&) = default;

        auto create_task() -> Task;
        auto player() const -> const Player &;
        auto level() const -> levels::LuaLevel *;

        virtual auto handle_key_press(const KeyEvent &) -> void override;
        virtual auto handle_level_complete(const std::string_view &name) -> void override;
        virtual auto handle_quit() -> void override;

    private:
        const Window &_window;
        messaging::MessageBus &_bus;
        Scheduler &_scheduler;
        Player _player;
        std::size_t _level_num;
        std::vector<ScriptLoader> _level_names;
        DefaultCache &_resource_cache;
        const TlvReader &_reader;
        std::unique_ptr<levels::LuaLevel> _level;
        bool _show_physics_debug;
        bool _show_debug;
        bool _running;
        messaging::AutoSubscribe _auto_subscribe;
    };
}

#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "game/levels/lua_level.h"
#include "game/player.h"
#include "game/routines/routine_base.h"
#include "messaging/auto_subscribe.h"
#include "messaging/message_bus.h"
#include "messaging/subscriber.h"
#include "physics/physics_sytem.h"
#include "resources/resource_cache.h"
#include "scheduler/scheduler.h"
#include "scheduler/task.h"
#include "scripting/script_loader.h"
#include "tlv/tlv_reader.h"
#include "window.h"

namespace game::routines
{
    class LevelRoutine : public RoutineBase
    {
    public:
        LevelRoutine(PhysicsSystem &ps, const Window &window, messaging::MessageBus &bus, Scheduler &scheduler, DefaultCache &resource_cache, const TlvReader &reader, const ResourceLoader &resource_loader);
        ~LevelRoutine() override = default;
        LevelRoutine(const LevelRoutine &) = delete;
        auto operator=(const LevelRoutine &) -> LevelRoutine & = delete;
        LevelRoutine(LevelRoutine &&) = default;

        auto create_task() -> Task;
        auto player() const -> const Player &;
        auto level() const -> levels::LuaLevel *;

        virtual auto handle_key_press(const KeyEvent &) -> void override;
        virtual auto handle_level_complete(const std::string_view &name) -> void override;

    private:
        PhysicsSystem &_ps;
        const Window &_window;
        Scheduler &_scheduler;
        Player _player;
        std::size_t _level_num;
        std::vector<ScriptLoader> _level_names;
        DefaultCache &_resource_cache;
        const ResourceLoader &_resource_loader;
        const TlvReader &_reader;
        std::unique_ptr<levels::LuaLevel> _level;
        bool _show_physics_debug;
        bool _show_debug;
    };
}

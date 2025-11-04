#pragma once

#include <span>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "game/levels/level.h"
#include "game/player.h"
#include "graphics/color.h"
#include "graphics/cube_map.h"
#include "messaging/auto_subscribe.h"
#include "messaging/message_bus.h"
#include "messaging/subscriber.h"
#include "physics/physics_sytem.h"
#include "physics/shape.h"
#include "primitives/entity.h"
#include "resources/resource_cache.h"
#include "resources/resource_loader.h"
#include "scripting/lua_script.h"
#include "scripting/script_loader.h"
#include "tlv/tlv_reader.h"

namespace game::levels
{

    enum class LevelState
    {
        PLAYING,
        COMPLETE,
        LOST
    };

    class LuaLevel : public Level, public messaging::Subscriber
    {
    public:
        struct BarrelInfo
        {
            Color tint_color;
            float tint_amount;
        };

        LuaLevel(
            PhysicsSystem &ps,
            const ScriptLoader &loader,
            DefaultCache &resource_cache,
            const ResourceLoader &resource_loader,
            const TlvReader &reader,
            const Player &player,
            messaging::MessageBus &bus);
        virtual ~LuaLevel() = default;

        auto update(Player &player) -> void override;
        auto restart() -> void override;

        auto entities() const -> std::span<const Entity>;

        auto physics() const -> const PhysicsSystem &;

        auto handle_entity_intersect(const Entity *a, const Entity *b) -> void override;
        auto handle_restart_level() -> void override;

    private:
        auto update_entity_visibility() -> void;
        auto update_level(Player &player) -> void;
        auto update_entity_collisions() -> void;

        PhysicsSystem &_ps;

        LuaScript _script;

        std::vector<Entity> _entities;
        std::vector<Entity> _level_entities;
        CubeMap _skybox;
        messaging::MessageBus &_bus;
        DefaultCache &_resource_cache;
        std::unordered_map<const Entity *, BarrelInfo> _barrel_info;
        std::vector<const Shape *> _shapes;

        messaging::AutoSubscribe _auto_subscribe;
    };
}
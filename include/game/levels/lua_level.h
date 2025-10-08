#pragma once

#include <span>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "game/levels/level.h"
#include "game/player.h"
#include "graphics/color.h"
#include "graphics/cube_map.h"
#include "graphics/texture_sampler.h"
#include "messaging/auto_subscribe.h"
#include "messaging/message_bus.h"
#include "messaging/subscriber.h"
#include "physics/physics_sytem.h"
#include "physics/shape.h"
#include "primitives/entity.h"
#include "resources/resource_cache.h"
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
            const ScriptLoader &loader,
            DefaultCache &resource_cache,
            const TlvReader &reader,
            const Player &player,
            messaging::MessageBus &bus);
        virtual ~LuaLevel() = default;

        auto update(const Player &player) -> void override;
        auto restart() -> void override;

        auto entities() const -> std::span<const Entity>;

        auto physics() const -> const PhysicsSystem &;

        auto handle_entity_intersect(const Entity *a, const Entity *b) -> void override;
        auto handle_restart_level() -> void override;

    private:
        PhysicsSystem _ps;

        LuaScript _script;

        std::vector<Entity> _entities;
        std::vector<Entity> _level_entities;
        CubeMap _skybox;
        TextureSampler _skybox_sampler;
        messaging::MessageBus &_bus;
        DefaultCache &_resource_cache;
        std::unordered_map<const Entity *, BarrelInfo> _barrel_info;
        std::vector<const Shape *> _shapes;

        messaging::AutoSubscribe _auto_subscribe;
    };
}
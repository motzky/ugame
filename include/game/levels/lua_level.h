#pragma once

#include <string_view>
#include <vector>

#include "game/levels/level.h"
#include "game/transformed_entity.h"
#include "resources/resource_cache.h"
#include "resources/resource_loader.h"
#include "scripting/lua_script.h"
#include "tlv/tlv_reader.h"

namespace game::levels
{
    class LuaLevel : public Level
    {
    public:
        LuaLevel(
            const game::ResourceLoader &loader,
            std::string_view script_name,
            DefaultCache &resource_cache,
            const TlvReader &reader,
            const Player &player);

        virtual auto update(const Player &player) -> void override;
        virtual auto restart() -> void override;

        auto entities() const -> const std::vector<TransformedEntity> &;

    private:
        LuaScript _script;

        std::vector<TransformedEntity> _entities;
        Entity _floor;
        CubeMap _skybox;
        TextureSampler _skybox_sampler;
        DefaultCache &_resource_cache;
    };
}
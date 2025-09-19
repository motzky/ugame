#pragma once

#include <span>
#include <vector>

#include "game/levels/level.h"
#include "game/player.h"
#include "game/transformed_entity.h"
#include "graphics/cube_map.h"
#include "graphics/scene.h"
#include "graphics/texture_sampler.h"
#include "messaging/message_bus.h"
#include "tlv/tlv_reader.h"

namespace game::levels
{
    class LevelApple : public Level
    {
    public:
        LevelApple(
            const Mesh *floor_mesh,
            const Material *floor_material,
            std::span<const Texture *> floor_textures,
            const Mesh *barrel_mesh,
            const Material *barrel_material,
            std::span<const Texture *> barrel_textures,
            const TlvReader &reader,
            const Player &player);

        ~LevelApple() override = default;

        virtual auto update(const Player &player) -> void override;
        virtual auto restart() -> void override;

        auto skybox() const -> const CubeMap &;

    private:
        std::vector<TransformedEntity> _entities;
        Entity _floor;
        CubeMap _skybox;
        TextureSampler _skybox_sampler;
        GameTransformState _state;
        // messaging::MessageBus &_bus;
    };
}
#include "game/levels/lua_level.h"

#include <algorithm>
#include <ranges>
#include <span>
#include <string_view>
#include <tuple>
#include <vector>

#include "game/player.h"
#include "graphics/cube_map.h"
#include "graphics/material.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/texture_sampler.h"
#include "messaging/message_bus.h"
#include "physics/box_shape.h"
#include "physics/physics_sytem.h"
#include "physics/transformed_shape.h"
#include "resources/resource_cache.h"
#include "resources/resource_loader.h"
#include "scripting/script_runner.h"
#include "tlv/tlv_reader.h"

using namespace std::literals;

namespace game::levels
{
    LuaLevel::LuaLevel(
        const game::ResourceLoader &loader,
        std::string_view script_name,
        DefaultCache &resource_cache,
        const TlvReader &reader,
        const Player &player,
        messaging::MessageBus &bus,
        PhysicsSystem &ps)
        : _script(loader.load(script_name).as_string()),
          _entities{},
          _floor{
              resource_cache.get<Mesh>("floor"),
              resource_cache.get<Material>("floor_material"),
              {0.f, -2.f, 0},
              {100.f, 1.f, 100.f},
              std::vector<const Texture *>{
                  resource_cache.get<Texture>("floor_albedo"),
                  resource_cache.get<Texture>("floor_albedo")}},
          _skybox{reader, {"skybox_right", "skybox_left", "skybox_top", "skybox_bottom", "skybox_front", "skybox_back"}},
          _skybox_sampler{},
          _bus(bus),
          _resource_cache(resource_cache),
          _barrel_info{},
          _ps{ps},
          _shapes{}
    {
        const auto runner = ScriptRunner{_script};
        runner.execute("Level_init_level", player.position());

        const Texture *barrel_textures[]{
            _resource_cache.get<Texture>("barrel_albedo"),
            _resource_cache.get<Texture>("barrel_specular"),
            _resource_cache.get<Texture>("barrel_normal"),
        };

        const auto barrel_count = runner.execute<std::int64_t>("Level_entity_count");
        for (std::int64_t i = 0; i < barrel_count; ++i)
        {
            const auto info = runner.execute<Vector3, Vector3, float>("Level_entity_info", i + 1);
            const auto &ent = _entities.emplace_back(Entity{_resource_cache.get<Mesh>("barrel"),
                                                            _resource_cache.get<Material>("barrel_material"),
                                                            std::get<0>(info),
                                                            {0.4f},
                                                            {{0.f}, {1.f}, {0.707107f, 0.f, 0.f, 0.707107f}},
                                                            barrel_textures});

            const auto aabb = ent.bounding_box();
            const auto dif = aabb.max - aabb.min;
            const auto dif_2 = dif / 2.f;
            const auto half_extents = dif_2; // + aabb.min;
            auto *shape = _ps.create_shape<BoxShape>(
                Vector3{
                    std::fabs(half_extents.x),
                    std::fabs(half_extents.y),
                    std::fabs(half_extents.z),
                });
            _shapes.push_back(shape);
        }

        _scene = Scene{
            .entities = _entities |
                        std::views::transform([](auto &e)
                                              { return std::addressof(e); }) |
                        std::ranges::to<std::vector>(),
            .ambient = {.r = .2f, .g = .2f, .b = .2f},
            .directional = {.direction = {-1.f, -1.f, -1.f}, .color = {.r = .3f, .g = .3f, .b = .3f}},
            .points = {{.position = {5.f, 3.f, 0.f}, .color = {.r = 1.f, .g = 0.f, .b = 0.f}, //
                        .const_attenuation = 1.f,
                        .linear_attenuation = .07f,
                        .quad_attenuation = 0.017f},
                       {.position = {-5.f, 3.f, 0.f}, .color = {.r = 0.f, .g = 1.f, .b = 0.f}, //
                        .const_attenuation = 1.f,
                        .linear_attenuation = .07f,
                        .quad_attenuation = 0.017f},
                       {.position = {-5.f, 3.f, 0.f}, .color = {.r = 0.f, .g = 0.f, .b = 1.f}, //
                        .const_attenuation = 1.f,
                        .linear_attenuation = .07f,
                        .quad_attenuation = 0.017f}},
            .debug_lines = {},
            .skybox = &_skybox,
            .skybox_sampler = &_skybox_sampler};

        _scene.entities.push_back(&_floor);
    }

    auto LuaLevel::update(const Player &player) -> void
    {
        const auto runner = ScriptRunner{_script};

        for (const auto &[index, entity] : std::views::enumerate(_entities))
        {
            runner.execute("Level_set_entity_visibility", index + 1, entity.is_visible());
        }

        runner.execute("Level_update_level", player.position());

        const auto transformed_shapes =
            std::views::zip_transform(
                [](const auto &shape, const auto &entitiy)
                { return TransformedShape{shape, entitiy.transform()}; },
                _shapes,
                _entities) |
            std::ranges::to<std::vector>();

        const auto max = std::ranges::size(transformed_shapes);
        auto combos = std::views::iota(0zu, max) |
                      std::views::transform([max](auto x)
                                            { return std::views::iota(x + 1, max) |
                                                     std::views::transform([x](auto y)
                                                                           { return std::make_pair(x, y); }); }) |
                      std::views::join;

        for (const auto &[index, entity] : std::views::enumerate(_entities))
        {
            const auto &[position, color, tint_amount] = runner.execute<Vector3, Vector3, float>("Level_entity_info", index + 1);
            entity.set_position(position);

            _barrel_info[std::addressof(entity)] = {.tint_color = {.r = color.x, .g = color.y, .b = color.z}, .tint_amount = tint_amount};

            for (const auto &[i, j] : combos)
            {
                const auto &transform_shape1 = transformed_shapes[i];
                const auto &transform_shape2 = transformed_shapes[j];

                transform_shape1.intersects(transform_shape2);
            }
        }

        if (runner.execute<bool>("Level_is_complete"))
        {
            const auto name = runner.execute<std::string>("Level_name");
            _bus.post_level_complete(name);
        }

        for (const auto &e : transformed_shapes)
        {
            e.draw(_ps.debug_renderer());
        }
    }

    auto LuaLevel::restart() -> void
    {
        const auto runner = ScriptRunner{_script};
        runner.execute("Level_restart_level");

        // read the reset entity values from LUA
        for (const auto &[index, entity] : std::views::enumerate(_entities))
        {
            const auto &[position, color, tint_amount] = runner.execute<Vector3, Vector3, float>("Level_entity_info", index + 1);
            entity.set_position(position);

            _barrel_info[std::addressof(entity)] = {.tint_color = {.r = color.x, .g = color.y, .b = color.z}, .tint_amount = tint_amount};

            const auto visibility = runner.execute<bool>("Level_entity_visibility", index + 1);
            entity.set_visibility(visibility);
        }

        _resource_cache.get<Material>("barrel_material")->set_uniform_callback([this](const auto *mat, const auto *ent)
                                                                               {
            if(const auto info = _barrel_info.find(ent); info != std::ranges::cend(_barrel_info))
            {
                mat->set_uniform("tint_color", info->second.tint_color);
                mat->set_uniform("tint_amount", info->second.tint_amount);

            } });
    }

    auto LuaLevel::entities() const -> std::span<const Entity>
    {
        return _entities;
    }

}
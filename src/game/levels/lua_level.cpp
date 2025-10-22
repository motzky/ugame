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
#include "graphics/text_factory.h"
#include "graphics/texture.h"
#include "graphics/texture_sampler.h"
#include "graphics/ui/label.h"
#include "messaging/message_bus.h"
#include "physics/box_shape.h"
#include "physics/physics_sytem.h"
#include "physics/transformed_shape.h"
#include "resources/resource_cache.h"
#include "resources/resource_loader.h"
#include "scripting/script_loader.h"
#include "scripting/script_runner.h"
#include "tlv/tlv_reader.h"
#include "utils/ensure.h"

using namespace std::literals;

namespace
{
    auto calculate_bounding_box(const game::Mesh *mesh, const game::Transform &transform) -> std::tuple<game::Vector3, game::Vector3>
    {
        const auto minmax_x = std::ranges::minmax(mesh->mesh_data().vertices, std::less<>{}, [](const auto &v)
                                                  { return v.position.x; });
        const auto minmax_y = std::ranges::minmax(mesh->mesh_data().vertices, std::less<>{}, [](const auto &v)
                                                  { return v.position.y; });
        const auto minmax_z = std::ranges::minmax(mesh->mesh_data().vertices, std::less<>{}, [](const auto &v)
                                                  { return v.position.z; });

        const auto transformed_min = game::Matrix4{transform} * game::Vector4{minmax_x.min.position.x, minmax_y.min.position.y, minmax_z.min.position.z, 1.0};
        const auto transformed_max = game::Matrix4{transform} * game::Vector4{minmax_x.max.position.x, minmax_y.max.position.y, minmax_z.max.position.z, 1.0};
        auto result = std::make_tuple(static_cast<game::Vector3>(transformed_min), static_cast<game::Vector3>(transformed_max));

        return result;
    }

    auto albedo_texture_name(std::string_view mesh_name) -> std::string
    {
        if (mesh_name == "Collums" ||
            mesh_name == "Concrete_wall_with_lines" ||
            mesh_name == "Roof" ||
            mesh_name == "Lower_roof")
        {
            return "Concrete042A_2K-JPG_Color";
        }
        if (mesh_name == "Window_frames" ||
            mesh_name == "Skylight_frame" ||
            mesh_name == "Skylight_frame.001" ||
            mesh_name == "Wall_beams" ||
            mesh_name == "Wall_beams.001" ||
            mesh_name == "Wall_beams.002" ||
            mesh_name == "Wall_beams.003" ||
            mesh_name == "Wall_beams.004" ||
            mesh_name == "Wall_beams.005" ||
            mesh_name == "Door_rail" ||
            mesh_name == "Sliding_door" ||
            mesh_name == "Sliding_door.001" ||
            mesh_name == "Sliding_door.002" ||
            mesh_name == "Sliding_door.003" ||
            mesh_name == "Sliding_door_frame" ||
            mesh_name == "Lower_roof_framing" ||
            mesh_name == "Roof_framing" ||
            mesh_name == "Roof_beams" ||
            mesh_name == "Roof_beams.001" ||
            mesh_name == "Window_Frame" ||
            mesh_name == "Window_frames" ||
            mesh_name == "Tap" ||
            mesh_name == "Lamps")
        {
            return "Iron_diffuse";
        }

        // if (mesh_name == "Main_floor")
        // {
        //     return "Floor diffuse";
        //     // return "Floor lines map";
        // }
        // if (mesh_name == "Main_walls")
        // {
        //     return "Main walls diffuse";
        // }
        // if (mesh_name == "")
        // {
        //     return "Blue line map";
        // }

        return "white";
    }

    auto normal_map_texture_name(std::string_view mesh_name) -> std::string
    {
        if (mesh_name == "Main_floor" ||
            mesh_name == "Main_walls" ||
            mesh_name == "Concrete_wall_with_lines" ||
            mesh_name == "Roof" ||
            mesh_name == "Lower_roof" ||
            mesh_name == "Collums")
        {
            return "Concrete042A_2K-JPG_NormalGL";
        }
        // if (mesh_name == "")
        // {
        //     return "Floor lines map";
        // }
        // if (mesh_name == "")
        // {
        //     return "Blue line map";
        // }
        if (mesh_name == "Window_frames" ||
            mesh_name == "Skylight_frame" ||
            mesh_name == "Skylight_frame.001" ||
            mesh_name == "Wall_beams" ||
            mesh_name == "Wall_beams.001" ||
            mesh_name == "Wall_beams.002" ||
            mesh_name == "Wall_beams.003" ||
            mesh_name == "Wall_beams.004" ||
            mesh_name == "Wall_beams.005" ||
            mesh_name == "Door_rail" ||
            mesh_name == "Sliding_door" ||
            mesh_name == "Sliding_door.001" ||
            mesh_name == "Sliding_door.002" ||
            mesh_name == "Sliding_door.003" ||
            mesh_name == "Sliding_door_frame" ||
            mesh_name == "Lower_roof_framing" ||
            mesh_name == "Roof_framing" ||
            mesh_name == "Roof_beams" ||
            mesh_name == "Roof_beams.001" ||
            mesh_name == "Tap" ||
            mesh_name == "Window_Frame" ||
            mesh_name == "Window_frames" ||
            mesh_name == "Lamps")
        {
            return "Metal025_2K-JPG_NormalGL";
        }

        return "barrel_normal";
    }

    auto material_name_from_mesh(std::string_view mesh_name) -> std::string
    {
        if (mesh_name == "Main_floor" ||
            mesh_name == "Main_walls")
        {
            return "level_material";
        }
        if (albedo_texture_name(mesh_name) == "white")
        {
            game::log::debug("returning checkerboard for mesh {}", mesh_name);
            return "checkerboard_material";
        }

        return "level_material";
    }
}

namespace game::levels
{
    LuaLevel::LuaLevel(
        const ScriptLoader &loader,
        DefaultCache &resource_cache,
        const ResourceLoader &resource_loader,
        const TlvReader &reader,
        const Player &player,
        messaging::MessageBus &bus)
        : _ps{},
          _script{loader.load()},
          _entities{},
          _level_entities{},
          _skybox{reader, {"skybox_right", "skybox_left", "skybox_top", "skybox_bottom", "skybox_front", "skybox_back"}},
          _bus(bus),
          _resource_cache(resource_cache),
          _barrel_info{},
          _shapes{},
          _auto_subscribe{_bus, {messaging::MessageType::ENTITY_INTERSECT, messaging::MessageType::RESTART_LEVEL}, this}
    {
        log::info("loading level {}", loader.name());

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
            const auto info = runner.execute<Vector3, Vector3, float, std::int64_t, std::int64_t>("Level_entity_info", i + 1);
            const auto *mesh = _resource_cache.get<Mesh>("barrel");
            const auto &[min, max] = calculate_bounding_box(mesh, {std::get<0>(info),
                                                                   {0.4f}});
            const auto half_extents = (max - min) / 2.f;
            auto *shape = _ps.create_shape<BoxShape>(
                Vector3{
                    std::fabs(half_extents.x),
                    std::fabs(half_extents.y),
                    std::fabs(half_extents.z),
                });
            _entities.emplace_back(Entity{mesh,
                                          _resource_cache.get<Material>("barrel_material"),
                                          std::get<0>(info),
                                          {0.4f},
                                          {{0.f}, {1.f}, {0.707107f, 0.f, 0.f, 0.707107f}},
                                          barrel_textures,
                                          TransformedShape{shape, {std::get<0>(info), {0.4f}, {0.707107f, 0.f, 0.f, 0.707107f}}},
                                          static_cast<std::uint32_t>(std::get<3>(info)),
                                          static_cast<std::uint32_t>(std::get<4>(info))});

            _shapes.push_back(shape);
        }

        const auto text_factory = TextFactory{resource_loader};
        static auto text_test = text_factory.create("Hello World!", _resource_cache.get<TextureSampler>("ui"), 16);
        static auto text_test2 = text_factory.create("Hello Corner!", _resource_cache.get<TextureSampler>("ui"), 16);

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
            .skybox_sampler = _resource_cache.get<TextureSampler>("sky_box"),
            .labels = {{0, 0, &text_test}, {1707, 0, &text_test2}},
            .effects = {.hdr = true, .grey_scale = false}};

        if (runner.has_function("Level_get_ambient"))
        {
            const auto ambient_vec = runner.execute<Vector3>("Level_get_ambient");
            _scene.ambient = {.r = ambient_vec.x, .g = ambient_vec.y, .b = ambient_vec.z};
        }
        if (runner.has_function("Level_get_direction_light"))
        {
            const auto [direction_light_dir, direction_light_color] = runner.execute<Vector3, Vector3>("Level_get_direction_light");
            _scene.directional = {.direction = direction_light_dir,
                                  .color = {
                                      .r = direction_light_color.x,
                                      .g = direction_light_color.y,
                                      .b = direction_light_color.z}};
        }

        const auto level_entity_names = std::vector{
            "Main_floor"sv,
            "Main_walls"sv,
            "Concrete_wall_with_lines"sv,
            "Collums"sv,
            "Roof"sv,
            "Window_frames"sv,
            "Window_Frame"sv,
            "Skylight_frame.001"sv,
            "Skylight_frame"sv,
            "Lightblock"sv,
            // "Glass_panes"sv,
            // "Skylight_glass_pane"sv,
            "Wall_beams"sv,
            "Wall_beams.001"sv,
            "Wall_beams.002"sv,
            "Wall_beams.003"sv,
            "Wall_beams.004"sv,
            "Wall_beams.005"sv,
            "Sliding_door"sv,
            "Door_rail"sv,
            "Sliding_door.001"sv,
            "Sliding_door.002"sv,
            "Sliding_door_frame"sv,
            "Sliding_door.003"sv,
            "Lower_roof"sv,
            "Roof_framing"sv,
            "Lower_roof_framing"sv,
            "Roof_beams.001"sv,
            "Roof_beams"sv,
            "Tap"sv,
            "Lamps"sv,
            "Light"sv,
        };

        _level_entities =
            level_entity_names |
            std::views::transform(
                [&](const auto e)
                { return Entity{resource_cache.get<Mesh>(e),
                                resource_cache.get<Material>(material_name_from_mesh(e)),
                                {0.f, -5.f, 0},
                                {10.f},
                                std::vector<const Texture *>{
                                    resource_cache.get<Texture>(albedo_texture_name(e)),
                                    resource_cache.get<Texture>(normal_map_texture_name(e))},
                                {_ps.create_shape<BoxShape>(Vector3{10.f}), {{0.f, -2.f, 0}, {1.f}, {}}},
                                0u,
                                0u}; }) |
            std::ranges::to<std::vector>();

        for (auto &ent : _level_entities)
        {
            _scene.entities.push_back(&ent);
        }

        restart();
    }

    auto LuaLevel::update(const Player &player) -> void
    {
        const auto runner = ScriptRunner{_script};

        for (const auto &[index, entity] : std::views::enumerate(_entities))
        {
            runner.execute("Level_set_entity_visibility", index + 1, entity.is_visible());
        }

        runner.execute("Level_update_level", player.position());

        auto orig_positions = _entities |
                              std::views::transform([](const auto &e)
                                                    { return std::make_tuple(false, e.position()); }) |
                              std::ranges::to<std::vector>();

        for (const auto &[index, entity] : std::views::enumerate(_entities))
        {
            const auto &[position, color, tint_amount, collsion_layer, collision_mask] =
                runner.execute<Vector3, Vector3, float, std::int64_t, std::int64_t>("Level_entity_info", index + 1);
            entity.set_position(position);

            _barrel_info[std::addressof(entity)] = {.tint_color = {.r = color.x, .g = color.y, .b = color.z}, .tint_amount = tint_amount};
        }

        const auto transformed_shapes =
            std::views::zip_transform(
                [](const auto &shape, const auto &entitiy)
                { return TransformedShape{shape, {entitiy.position(), {1.f}, {0.707107f, 0.f, 0.f, 0.707107f}}}; },
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

        for (const auto &[i, j] : combos)
        {
            const auto ent1 = _entities[i];
            const auto ent2 = _entities[j];

            if ((ent1.collision_mask() & ent2.collision_layer()) && (ent2.collision_mask() & ent1.collision_layer()))
            {
                const auto &transform_shape1 = transformed_shapes[i];
                const auto &transform_shape2 = transformed_shapes[j];

                if (transform_shape1.intersects(transform_shape2))
                {
                    std::get<0>(orig_positions[i]) = true;
                    std::get<0>(orig_positions[j]) = true;
                    _bus.post_entity_intersect(std::addressof(_entities[i]), std::addressof(_entities[j]));
                }
            }
        }

        const auto level_state = static_cast<LevelState>(runner.execute<std::int64_t>("Level_state"));
        switch (level_state)
        {
            using enum LevelState;
        case COMPLETE:
        {
            const auto name = runner.execute<std::string>("Level_name");
            _bus.post_level_complete(name);
        }
        break;
        case LOST:
            _bus.post_restart_level();
            break;
        default:
        {

            for (const auto &[index, orig] : std::views::enumerate(orig_positions))
            {
                if (const auto &[revert, orig_position] = orig; revert)
                {
                    _entities[index].set_position(orig_position);
                    runner.execute("Level_set_entity_position", index + 1, orig_position);
                }
            }
            if (runner.has_function("Level_get_ambient"))
            {
                const auto ambient_vec = runner.execute<Vector3>("Level_get_ambient");
                _scene.ambient = {.r = ambient_vec.x, .g = ambient_vec.y, .b = ambient_vec.z};
            }
            if (runner.has_function("Level_get_direction_light"))
            {
                const auto [direction_light_dir, direction_light_color] = runner.execute<Vector3, Vector3>("Level_get_direction_light");
                _scene.directional = {.direction = direction_light_dir,
                                      .color = {
                                          .r = direction_light_color.x,
                                          .g = direction_light_color.y,
                                          .b = direction_light_color.z}};
            }
        }
        break;
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
            const auto &[position, color, tint_amount, collision_layer, collision_mask] = runner.execute<Vector3, Vector3, float, std::int64_t, std::int64_t>("Level_entity_info", index + 1);
            entity.set_position(position);

            // Mabybe not needed, unless we introduce tint animations
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

    auto LuaLevel::physics() const -> const PhysicsSystem &
    {
        return _ps;
    }

    auto LuaLevel::handle_entity_intersect(const Entity *a, const Entity *b) -> void
    {
        const auto *begin = _entities.data();
        const auto index_a = static_cast<std::int64_t>(a - begin);
        const auto index_b = static_cast<std::int64_t>(b - begin);

        expect(index_a >= 0 && index_a < static_cast<std::int64_t>(_entities.size()), "index_a {} out of range", index_a);
        expect(index_b >= 0 && index_b < static_cast<std::int64_t>(_entities.size()), "index_a {} out of range", index_b);

        const auto runner = ScriptRunner{_script};
        runner.execute("Level_handle_entity_intersect", index_a + 1, index_b + 1);
    }

    auto LuaLevel::handle_restart_level() -> void
    {
        restart();
    }
}
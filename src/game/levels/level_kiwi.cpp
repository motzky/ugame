#include "game/levels/level_kiwi.h"

#include <array>
#include <ranges>
#include <span>
#include <vector>

#include "game/transformed_entity.h"
#include "graphics/color.h"
#include "graphics/cube_map.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "math/aabb.h"
#include "math/frustum_plane.h"

namespace
{
    auto intersects_frustum(const game::AABB &aabb, const std::array<game::FrustumPlane, 6u> &planes) -> bool
    {
        for (const auto &plane : planes)
        {

            auto pos_vert = aabb.get_min();
            if (plane.normal.x >= 0)
            {
                pos_vert.x = aabb.get_max().x;
            }
            if (plane.normal.y >= 0)
            {
                pos_vert.y = aabb.get_max().y;
            }
            if (plane.normal.z >= 0)
            {
                pos_vert.z = aabb.get_max().z;
            }

            if (game::Vector3::dot(plane.normal, pos_vert) + plane.distance < 0.f)
            {
                return false;
            }
        }
        return true;
    }
    constexpr auto CameraDelta = [](const game::Vector3 &in, const game::GameTransformState &state) -> game::TransformerResult
    { return {in + state.camera.position() - state.camera_last_position}; };

    constexpr auto Invert = [](const game::Vector3 &in, const game::GameTransformState &) -> game::TransformerResult
    { return {-in}; };

    constexpr auto CheckVisible = [](const game::Vector3 &in, const game::GameTransformState &state) -> game::TransformerResult
    {
        const auto planes = state.camera.frustum_planes();
        return {-in, !intersects_frustum(state.aabb, planes)};
    };
}

namespace game::levels
{
    LevelKiwi::LevelKiwi(
        DefaultCache &resource_cache,
        const TlvReader &reader,
        const Player &player,
        messaging::MessageBus &bus)
        : _entities{},
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
          _state{.camera = player.camera(), .aabb = {}, .camera_last_position = player.position()},
          _bus(bus),
          _resource_cache(resource_cache)
    {

        const Texture *barrel_textures[]{
            resource_cache.get<Texture>("barrel_albedo"),
            resource_cache.get<Texture>("barrel_specular"),
            resource_cache.get<Texture>("barrel_normal"),
        };

        _entities.emplace_back(game::Entity{_resource_cache.get<Mesh>("barrel"), resource_cache.get<Material>("barrel_material"), {0.f, -.2f, 0.f}, {0.4f}, {{0.f}, {1.f}, {0.707107f, 0.f, 0.f, 0.707107f}}, barrel_textures},
                               game::AABB{{-.6f, -.75f, -.6f}, {.6f, .75f, .6f}, {0.f, -.2f, 0.f}},
                               std::make_unique<game::Chain<GameTransformState>>());
        _entities.emplace_back(game::Entity{_resource_cache.get<Mesh>("barrel"), resource_cache.get<Material>("barrel_material"), {5.f, -.2f, 0.f}, {0.4f}, {{0.f}, {1.f}, {0.707107f, 0.f, 0.f, 0.707107f}}, barrel_textures},
                               game::AABB{{4.4f, -.75f, -.6f}, {5.6f, .75f, .6f}, {5.f, -.2f, 0.f}},
                               std::make_unique<game::Chain<GameTransformState, CheckVisible, CameraDelta, Invert>>());

        _scene = game::Scene{
            .entities = _entities |
                        std::views::transform([](const auto &e)
                                              { return std::addressof(e.entity); }) |
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

    auto LevelKiwi::update(const Player &player) -> void
    {
        for (const auto &[transformed_entity, light] : std::views::zip(_entities, _scene.points))
        {
            auto &[entity, aabb, transformer] = transformed_entity;
            _state.aabb = aabb;
            if (!_show_debug)
            {
                const auto entity_delta = transformer->go({}, _state);
                entity.translate(entity_delta);

                aabb.set_position(entity.position());
            }

            // debug_wireframe_renderer.draw(aabb);

            const auto position = entity.position();
            light.position = {position.x, 5.f, position.z};
        }

        _state.camera_last_position = player.position();

        if (Vector3::distance(_entities[0].entity.position(), _entities[1].entity.position()) < 1.f)
        {
            _bus.post_level_complete("kiwi");
        }
    }

    auto LevelKiwi::restart() -> void
    {
        _resource_cache.get<Material>("barrel_material")->set_uniform_callback([this](const Material *material, const Entity *entity)
                                                                               {
                const auto tint_amount = entity == std::addressof(_entities[0].entity) ? .2f : .05f;
                material->set_uniform("tint_color", Color{.r = 0.0f, .g = 0.0f, .b = 1.0f});
                material->set_uniform("tint_amount", tint_amount); });
    }

    auto LevelKiwi::skybox() const -> const CubeMap &
    {
        return _skybox;
    }
}

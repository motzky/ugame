#include "game/levels/level_apple.h"

#include <array>
#include <ranges>
#include <span>
#include <vector>

#include "game/transformed_entity.h"
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

            auto pos_vert = aabb.min;
            if (plane.normal.x >= 0)
            {
                pos_vert.x = aabb.max.x;
            }
            if (plane.normal.y >= 0)
            {
                pos_vert.y = aabb.max.y;
            }
            if (plane.normal.z >= 0)
            {
                pos_vert.z = aabb.max.z;
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

    // auto textures() -> std::vector<const game::Texture *>
    // {
    //     return {};
    // }
}

namespace game::levels
{
    LevelApple::LevelApple(
        const Mesh *floor_mesh,
        const Material *floor_material,
        std::span<const Texture *> floor_textures,
        const Mesh *barrel_mesh,
        const Material *barrel_material,
        std::span<const Texture *> barrel_textures,
        const TlvReader &reader,
        const Player &player)
        : _entities{},
          _floor{game::Entity{floor_mesh, floor_material, {0.f, -2.f, 0}, {100.f, 1.f, 100.f}, floor_textures}},
          _skybox{reader, {"skybox_right", "skybox_left", "skybox_top", "skybox_bottom", "skybox_front", "skybox_back"}},
          _skybox_sampler{},
          _state{.camera = player.camera(), .aabb = {}, .camera_last_position = player.position()}
    {

        _entities.emplace_back(game::Entity{barrel_mesh, barrel_material, {-5.f, -.2f, 0.f}, {0.4f}, {{0.f}, {1.f}, {0.707107f, 0.f, 0.f, 0.707107f}}, barrel_textures},
                               game::AABB{{-5.6f, -.75f, -.6f}, {-4.4f, .75f, .6f}},
                               std::make_unique<game::Chain<GameTransformState, CheckVisible, CameraDelta, Invert>>());
        _entities.emplace_back(game::Entity{barrel_mesh, barrel_material, {0.f, -.2f, 0.f}, {0.4f}, {{0.f}, {1.f}, {0.707107f, 0.f, 0.f, 0.707107f}}, barrel_textures},
                               game::AABB{{-.6f, -.75f, -.6f}, {.6f, .75f, .6f}},
                               std::make_unique<game::Chain<GameTransformState>>());
        _entities.emplace_back(game::Entity{barrel_mesh, barrel_material, {5.f, -.2f, 0.f}, {0.4f}, {{0.f}, {1.f}, {0.707107f, 0.f, 0.f, 0.707107f}}, barrel_textures},
                               game::AABB{{4.4f, -.75f, -.6f}, {5.6f, .75f, .6f}},
                               std::make_unique<game::Chain<GameTransformState, CheckVisible, CameraDelta>>());

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

    auto LevelApple::update(const Player &player) -> void
    {
        // for (const auto &[transformed_entity, light] : std::views::zip(entities, scene.points))
        // {
        //     auto &[entity, aabb, transformer] = transformed_entity;
        //     state.aabb = aabb;
        //     if (!show_debug)
        //     {
        //         const auto enitiy_delta = transformer->go({}, state);
        //         entity.translate(enitiy_delta);

        //         aabb.min += enitiy_delta;
        //         aabb.max += enitiy_delta;
        //     }

        //     debug_wireframe_renderer.draw(aabb);

        //     const auto position = entity.position();
        //     light.position = {position.x, 5.f, position.z};
        // }

        _state.camera_last_position = player.position();
    }

    auto LevelApple::restart() -> void
    {
    }

    auto LevelApple::skybox() const -> const CubeMap &
    {
        return _skybox;
    }
}

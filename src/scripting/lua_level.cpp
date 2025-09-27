#include "game/levels/lua_level.h"

#include <string_view>

#include "game/player.h"
#include "graphics/cube_map.h"
#include "graphics/material.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/texture_sampler.h"
#include "resources/resource_cache.h"
#include "resources/resource_loader.h"
#include "scripting/script_runner.h"
#include "tlv/tlv_reader.h"

namespace game::levels
{
    LuaLevel::LuaLevel(const game::ResourceLoader &loader, std::string_view script_name, DefaultCache &resource_cache, const TlvReader &reader)
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
          _resource_cache(resource_cache)
    {
        const auto runner = ScriptRunner{_script};
        runner.execute("init_level");

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

    auto LuaLevel::update(const Player &player) -> void
    {
        const auto runner = ScriptRunner{_script};
        runner.execute("update_level", player.position());
    }

    auto LuaLevel::restart() -> void
    {
        const auto runner = ScriptRunner{_script};
        runner.execute("restart_level");
    }

}
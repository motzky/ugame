#include "game/game.h"

#include <array>
#include <print>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>

// #ifndef WIN32
// #include <GLFW/glfw3.h>
// #endif

#include "game/routines/input_routine.h"
#include "game/routines/level_routine.h"
#include "game/routines/render_routine.h"
#include "graphics/cube_map.h"
#include "graphics/material.h"
#include "graphics/shader.h"
#include "graphics/texture.h"
#include "graphics/texture_sampler.h"
#include "loaders/mesh_loader.h"
#include "log.h"
#include "messaging/message_bus.h"
#include "resources/resource_cache.h"
#include "resources/resource_loader.h"
#include "scheduler/scheduler.h"
#include "tlv/tlv_entry.h"
#include "tlv/tlv_reader.h"
#include "utils/decompress.h"
#include "window.h"

using namespace std::string_view_literals;

namespace
{
    auto get_uint_arg(const std::vector<std::string_view> &args, std::string_view arg_name, std::uint32_t defval = 0u) -> std::uint32_t
    {
        const auto arg = std::ranges::find(args, arg_name);
        if (arg == std::ranges::cend(args))
        {
            return defval;
        }

        const auto index = static_cast<std::size_t>(std::ranges::distance(std::ranges::cbegin(args), arg));
        if (index + 1u >= args.size())
        {
            return defval;
        }

        return std::stol(args[index + 1].data());
    }
}

namespace game
{
    Game::Game(const std::vector<std::string_view> &args)
        : _running(true),
          _message_bus{},
          _window{
              get_uint_arg(args, "-width"sv, 1920u),
              get_uint_arg(args, "-height"sv, 1080u),
              get_uint_arg(args, "-x"sv),
              get_uint_arg(args, "-y"sv)}
    {
    }

    Game::~Game() = default;

    auto Game::run(std::string_view resource_root) -> void
    {
        auto mesh_loader = game::MeshLoader{};
        auto resource_cache = game::DefaultCache{};

        const auto *sampler = resource_cache.insert<game::TextureSampler>("default");

        game::log::info("loading resources...");
        auto resource_loader = game::ResourceLoader{resource_root};

        game::log::info("decompressing...");
        const auto tlv_file = [&resource_loader]
        {
            const auto compressed_tlv_file = resource_loader.load("resources");
            return game::decompress(compressed_tlv_file.as_bytes());
        }();

        const auto reader = game::TlvReader{tlv_file};

        game::log::info("Loading meshes...");
        resource_cache.insert<Mesh>("barrel", reader, "Cylinder.014");
        resource_cache.insert<Mesh>("floor", mesh_loader.cube());

        game::log::info("Creating materials...");

        const auto vertex_shader_file = TlvReader::get_text_file(reader, "simple.vert");
        const auto fragment_shader_file = TlvReader::get_text_file(reader, "barrel.frag");

        const auto vertex_shader = game::Shader{vertex_shader_file.data, game::ShaderType::VERTEX};
        const auto fragment_shader = game::Shader{fragment_shader_file.data, game::ShaderType::FRAGMENT};
        resource_cache.insert<Material>("barrel_material", vertex_shader, fragment_shader);

        const auto checker_vertex_shader_file = TlvReader::get_text_file(reader, "simple.vert");
        const auto checker_fragment_shader_file = TlvReader::get_text_file(reader, "checker.frag");
        const auto checker_vertex_shader = game::Shader{checker_vertex_shader_file.data, game::ShaderType::VERTEX};
        const auto checker_fragment_shader = game::Shader{checker_fragment_shader_file.data, game::ShaderType::FRAGMENT};
        resource_cache.insert<Material>("floor_material", checker_vertex_shader, checker_fragment_shader);

        game::log::info("Creating GL textures...");

        resource_cache.insert<game::Texture>("barrel_albedo", reader, "barrel_base_albedo", sampler);
        resource_cache.insert<game::Texture>("barrel_specular", reader, "barrel_metallic", sampler);
        resource_cache.insert<game::Texture>("barrel_normal", reader, "barrel_normal_ogl", sampler);

        resource_cache.insert<Texture>(
            "floor_albedo",
            game::TextureDescription{
                .name = "white",
                .format = game::TextureFormat::RGB,
                .usage = game::TextureUsage::SRGB,
                .width = 1u,
                .height = 1u,
                .data{static_cast<std::byte>(0xff), static_cast<std::byte>(0xff), static_cast<std::byte>(0xff)}},
            sampler);

        game::log::info("Setting up scheduler...");
        auto scheduler = Scheduler{};

        auto input_routine = routines::InputRoutine{_window, _message_bus, scheduler};
        scheduler.add(input_routine.create_task());

        auto level_routine = routines::LevelRoutine{_window, _message_bus, scheduler, resource_cache, reader};
        scheduler.add(level_routine.create_task());

        auto render_routine = routines::RenderRoutine{level_routine, _window, _message_bus, scheduler, reader, mesh_loader};
        scheduler.add(render_routine.create_task());

        game::log::info("Running scheduler...");
        scheduler.run();
    }

}
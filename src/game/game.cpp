#include "game/game.h"

#include <ranges>
#include <string>
#include <string_view>
#include <utility>

#include "game/routines/input_routine.h"
#include "game/routines/level_routine.h"
#include "game/routines/main_menu_routine.h"
#include "game/routines/render_routine.h"
#include "game/routines/sound_routine.h"
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
#include "sound/sound_data.h"
#include "tlv/tlv_entry.h"
#include "tlv/tlv_reader.h"
#include "utils/decompress.h"
#include "window.h"

using namespace std::literals;

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

    auto get_uint8_arg(const std::vector<std::string_view> &args, std::string_view arg_name, std::uint8_t defval = 0u) -> std::uint8_t
    {
        auto u32 = get_uint_arg(args, arg_name, defval);
        if (u32 > 255)
        {
            return defval;
        }
        return static_cast<std::uint8_t>(u32);
    }

    auto load_sub_meshes_into_cache(game::DefaultCache &resource_cache, std::string_view obj_name, const game::TlvReader &reader) -> void
    {

        const auto data = std::ranges::find_if(reader, [obj_name](const auto &e)
                                               { return e.is_object_data(obj_name); });
        game::ensure(data != std::ranges::end(reader), "failed to load meshes of object '{}'", obj_name);

        auto mesh_names = (*data).object_data_value();

        game::log::info("loading {} meshes for object {}", mesh_names.size(), obj_name);

        for (const auto &str : mesh_names)
        {
            // game::log::debug("loading mesh {}", str);
            resource_cache.insert<game::Mesh>(str, reader, str);
        }
    }

}

namespace game
{
    Game::Game(const std::vector<std::string_view> &args)
        : _running(true),
          _message_bus{},
          _samples{get_uint8_arg(args, "-m"sv, 1u)},
          _window{
              get_uint_arg(args, "-width"sv, 1920u),
              get_uint_arg(args, "-height"sv, 1080u),
              get_uint_arg(args, "-x"sv),
              get_uint_arg(args, "-y"sv),
              _samples}
    {
    }

    Game::~Game() = default;

    auto Game::run(std::string_view resource_root) -> void
    {
        auto mesh_loader = game::MeshLoader{};
        auto resource_cache = game::DefaultCache{};

        const auto *mipmap = resource_cache.insert<game::TextureSampler>("mipmap", TextureSampler{FilterType::LINEAR_MIPMAP, FilterType::NEAREST, std::make_optional(16.f)});
        resource_cache.insert<TextureSampler>("sky_box", TextureSampler{FilterType::NEAREST, FilterType::NEAREST});
        resource_cache.insert<TextureSampler>("ui", TextureSampler{FilterType::NEAREST, FilterType::NEAREST});

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

        load_sub_meshes_into_cache(resource_cache, "SHC factory hall renovated", reader);

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
        resource_cache.insert<Material>("checkerboard_material", checker_vertex_shader, checker_fragment_shader);

        resource_cache.insert<Material>("level_material", vertex_shader, fragment_shader);

        const auto simple_fragment_shader_file = TlvReader::get_text_file(reader, "simple.frag");
        const auto simple_fragment_shader = game::Shader{simple_fragment_shader_file.data, game::ShaderType::FRAGMENT};
        resource_cache.insert<Material>("floor_material", vertex_shader, simple_fragment_shader);

        game::log::info("Creating GL textures...");

        resource_cache.insert<Texture>("barrel_albedo", reader, "barrel_base_albedo", mipmap);
        resource_cache.insert<Texture>("barrel_specular", reader, "barrel_metallic", mipmap);
        resource_cache.insert<Texture>("barrel_normal", reader, "barrel_normal_ogl", mipmap);

        resource_cache.insert<Texture>("Concrete042A_2K-JPG_Color", reader, "Concrete042A_2K-JPG_Color", mipmap);
        resource_cache.insert<Texture>("Concrete042A_2K-JPG_NormalGL", reader, "Concrete042A_2K-JPG_NormalGL", mipmap);
        resource_cache.insert<Texture>("Metal025_2K-JPG_NormalGL", reader, "Metal025_2K-JPG_NormalGL", mipmap);
        resource_cache.insert<Texture>("Floor lines map", reader, "Floor lines map", mipmap);
        resource_cache.insert<Texture>("Blue line map", reader, "Blue line map", mipmap);
        resource_cache.insert<Texture>("Floor diffuse", reader, "Floor diffuse", mipmap);
        resource_cache.insert<Texture>("Main walls diffuse", reader, "Main walls diffuse", mipmap);

        resource_cache.insert<Texture>("Iron_diffuse",
                                       TextureDescription{
                                           .name = "Iron",
                                           .format = TextureFormat::RGB,
                                           .usage = TextureUsage::SRGB,
                                           .width = 1u,
                                           .height = 1u,
                                           .data = {static_cast<std::byte>(0), static_cast<std::byte>(33), static_cast<std::byte>(105)}},
                                       mipmap);

        resource_cache.insert<Texture>(
            "white",
            game::TextureDescription{
                .name = "white",
                .format = game::TextureFormat::RGB,
                .usage = game::TextureUsage::SRGB,
                .width = 1u,
                .height = 1u,
                .data = {static_cast<std::byte>(0xff), static_cast<std::byte>(0xff), static_cast<std::byte>(0xff)}},
            mipmap);

        const auto main_theme_data = std::ranges::find_if(reader, [](const auto &entry)
                                                          { return entry.is_sound("main_theme.wav"); });
        if (main_theme_data == std::ranges::cend(reader))
        {
            log::error("Could not find sound: main theme");
        }
        else
        {
            resource_cache.insert<SoundData>("main_theme", (*main_theme_data).sound_data_value());
        }

        game::log::info("Setting up scheduler...");
        auto scheduler = Scheduler{_message_bus};

        auto input_routine = routines::InputRoutine{_window, _message_bus, scheduler};
        auto level_routine = routines::LevelRoutine{_window, _message_bus, scheduler, resource_cache, reader, resource_loader};
        auto render_routine = routines::RenderRoutine{_window, _message_bus, scheduler, reader, mesh_loader, _samples};
        auto sound_routine = routines::SoundRoutine{_message_bus, scheduler, resource_cache};

        // FIXME: has to be last, because it sends messages in constructor
        auto main_menu_routine = routines::MainMenuRoutine{_window, _message_bus, scheduler, resource_cache, reader, resource_loader};

        scheduler.add(input_routine.create_task());
        scheduler.add(sound_routine.create_task());
        scheduler.add(main_menu_routine.create_task());
        scheduler.add(level_routine.create_task());
        scheduler.add(render_routine.create_task());

        game::log::info("Running scheduler...");
        scheduler.run();
    }
}
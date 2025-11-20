#include "game/routines/main_menu_routine.h"

#include <cmath>
#include <coroutine>
#include <format>
#include <numbers>
#include <string>
#include <string_view>
#include <vector>

#include "config.h"
#include "game/routines/routine_base.h"
#include "graphics/camera.h"
#include "graphics/scene.h"
#include "graphics/text_factory.h"
#include "graphics/ui/label.h"
#include "log.h"
#include "messaging/message_bus.h"
#include "messaging/subscriber.h"
#include "resources/resource_cache.h"
#include "scheduler/scheduler.h"
#include "scheduler/task.h"
#include "scheduler/wait.h"
#include "tlv/tlv_reader.h"
#include "utils/string_unordered_map.h"

using namespace std::literals;

namespace
{
    auto create_camera(const game::Window &window) -> game::Camera
    {
        return {{0.f, 1.5f, 10.f},
                {0.f, 0.f, 0.f},
                {0.f, 1.f, 0.f},
                std::numbers::pi_v<float> / 4.f,
                static_cast<float>(window.width()),
                static_cast<float>(window.height()),
                0.1f,
                500.f};
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

    auto specular_texture_name(std::string_view mesh_name) -> std::string
    {
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
            return "Iron_specular";
        }

        return albedo_texture_name(mesh_name);
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

namespace game::routines
{
    MainMenuRoutine::MainMenuRoutine(const Window &window, messaging::MessageBus &bus, Scheduler &scheduler, DefaultCache &resource_cache, const TlvReader &reader,
                                     const ResourceLoader &resource_loader)
        : RoutineBase{bus, {messaging::MessageType::KEY_PRESS}},
          _window{window},
          _scheduler{scheduler},
          _resource_cache{resource_cache},
          _entities{},
          _level_entities{},
          _labels{},
          _resource_loader{resource_loader},
          _reader{reader},
          _camera(create_camera(window)),
          _scene{}
    {
        _window.set_title("game: MainMenu");

        const Texture *barrel_textures[]{
            _resource_cache.get<Texture>("barrel_albedo"),
            _resource_cache.get<Texture>("barrel_specular"),
            _resource_cache.get<Texture>("barrel_normal"),
        };
        const auto *mesh = _resource_cache.get<Mesh>("barrel");
        _entities.emplace_back(Entity{mesh,
                                      _resource_cache.get<Material>("barrel_material"),
                                      Vector3{0.f},
                                      {0.4f},
                                      {{0.f}, {1.f}, {0.707107f, 0.f, 0.f, 0.707107f}},
                                      barrel_textures,
                                      TransformedShape{nullptr, {Vector3{0.0f, -0.2f, 0.0f}, {0.4f}, {0.707107f, 0.f, 0.f, 0.707107f}}},
                                      0u,
                                      0u});

        const auto text_factory = TextFactory{resource_loader};

        _labels.push_back(text_factory.create("Barrel Game", _resource_cache.get<TextureSampler>("ui"), 32));
        _labels.push_back(text_factory.create("Press any key to start", _resource_cache.get<TextureSampler>("ui"), 24));
        _labels.push_back(text_factory.create("or ESC to exit", _resource_cache.get<TextureSampler>("ui"), 24));
        _labels.push_back(text_factory.create(std::format("v{}.{}.{}", version::major, version::minor, version::patch), _resource_cache.get<TextureSampler>("ui"), 16));

        const auto ambient_vec = Vector3{0.15f};
        const auto direction_light_dir = Vector3{0.f, 0.f, 1.f};
        const auto direction_light_color = Vector3{0.2f};

        _scene = Scene{
            .entities = _entities |
                        std::views::transform([](auto &e)
                                              { return std::addressof(e); }) |
                        std::ranges::to<std::vector>(),
            .ambient = {.r = ambient_vec.x, .g = ambient_vec.y, .b = ambient_vec.z},
            .directional =
                {.direction = direction_light_dir,
                 .color = {.r = direction_light_color.x, .g = direction_light_color.y, .b = direction_light_color.z}},
            .points = {{.position = {4.f, 2.f, 0.f},
                        .color = {.r = 1.f, .g = 0.f, .b = 0.f},
                        .const_attenuation = 1.f,
                        .linear_attenuation = .07f,
                        .quad_attenuation = 0.017f},
                       {.position = {-4.f, 2.f, 0.f},
                        .color = {.r = 0.f, .g = 1.f, .b = 0.f},
                        .const_attenuation = 1.f,
                        .linear_attenuation = .07f,
                        .quad_attenuation = 0.017f},
                       {.position = {0.f, 2.f, 4.f},
                        .color = {.r = 0.f, .g = 0.f, .b = 1.f},
                        .const_attenuation = 1.f,
                        .linear_attenuation = .07f,
                        .quad_attenuation = 0.017f}},
            .debug_lines = {},
            .skybox = nullptr,
            .skybox_sampler = _resource_cache.get<TextureSampler>("sky_box"),
            .labels = {
                {window.width() / 2 - _labels[0].width() / 2, 360, &_labels[0], Color::red()},
                {window.width() / 2 - _labels[1].width() / 2, 900, &_labels[1], Color::green()},
                {window.width() / 2 - _labels[2].width() / 2, 930, &_labels[2], Color::green()},
                {5u, window.height() - 17u, &_labels[3], Color::white()}},
            .effects = {.hdr = true, .grey_scale = false, .blur = false, .ssao = true}};

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
                                {-218.f, -3.2f, 70},
                                {10.f},
                                std::vector<const Texture *>{
                                    resource_cache.get<Texture>(albedo_texture_name(e)),
                                    resource_cache.get<Texture>(specular_texture_name(e)),
                                    resource_cache.get<Texture>(normal_map_texture_name(e))},
                                {nullptr, {{0.f, -2.f, 0}, {1.f}, {}}},
                                0u,
                                0u}; }) |
            std::ranges::to<std::vector>();

        for (auto &ent : _level_entities)
        {
            _scene.entities.push_back(&ent);
        }

        _bus.post_change_camera(&_camera);
        _bus.post_change_scene(&_scene);
    }

    auto MainMenuRoutine::create_task() -> Task
    {
        auto theta = 0.f;

        const auto pi2 = std::numbers::pi_v<float> * 2.0;
        const auto rpm = 30;
        const auto delta_theta = rpm / 3600.f;
        const auto radius = 10.f;

        while (_state != GameState::EXITING)
        {
            if (_state != GameState::MAIN_MENU)
            {
                co_await Wait{_scheduler, GameState::MAIN_MENU};
            }

            theta += delta_theta;
            if (theta > pi2)
            {
                theta -= pi2;
            }

            const auto translation = Vector3{
                radius * std::sin(theta),
                _camera.position().y,
                radius * std::cos(theta)};

            _camera.set_position(translation);

            _camera.adjust_yaw(-delta_theta);

            _camera.update();

            if (_state != GameState::EXITING)
            {
                co_await Wait{_scheduler, 16666us};
            }
        }
        log::info("MainMenuRoutine ending");
    }

    auto MainMenuRoutine::handle_key_press(const KeyEvent &event) -> void
    {
        if (_state == GameState::MAIN_MENU)
        {
            if (event.key() == Key::ESC && event.state() == KeyState::UP)
            {
                _bus.post_state_change(GameState::EXITING);
                return;
            }
            if (event.state() == KeyState::UP)
            {
                _bus.post_state_change(GameState::RUNNING);
            }
        }
    }
}
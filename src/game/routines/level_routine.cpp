#include "game/routines/level_routine.h"

#include <coroutine>
#include <numbers>
#include <string>
#include <vector>

#include "camera.h"
#include "game/levels/level.h"
#include "log.h"
#include "messaging/message_bus.h"
#include "messaging/subscriber.h"
#include "physics/box_shape.h"
#include "primitives/entity.h"
#include "resources/resource_cache.h"
#include "scheduler/scheduler.h"
#include "scheduler/task.h"
#include "scheduler/wait.h"
#include "scripting/script_loader.h"
#include "tlv/tlv_reader.h"

using namespace std::string_view_literals;

namespace
{
    auto intersects_frustum(const game::TransformedShape &bounding_box, const std::array<game::FrustumPlane, 6u> &planes) -> bool
    {
        game::expect(bounding_box.shape()->type() == game::ShapeType::BOX, "Not a bounding BOX");

        const auto *box_shape = dynamic_cast<const game::BoxShape *>(bounding_box.shape());
        const auto position = bounding_box.transform().position;

        const auto min = position - box_shape->dimensions();
        const auto max = position + box_shape->dimensions();

        for (const auto &plane : planes)
        {
            auto pos_vert = min;
            if (plane.normal.x >= 0)
            {
                pos_vert.x = max.x;
            }
            if (plane.normal.y >= 0)
            {
                pos_vert.y = max.y;
            }
            if (plane.normal.z >= 0)
            {
                pos_vert.z = max.z;
            }

            if (game::Vector3::dot(plane.normal, pos_vert) + plane.distance < 0.f)
            {
                return false;
            }
        }
        return true;
    }

    auto create_camera(const game::Window &window) -> game::Camera
    {
        return {{0.f, 2.f, 20.f},
                {0.f, 0.f, 0.f},
                {0.f, 1.f, 0.f},
                std::numbers::pi_v<float> / 4.f,
                static_cast<float>(window.width()),
                static_cast<float>(window.height()),
                0.1f,
                500.f};
    }

    auto get_level_loaders(const game::TlvReader &reader) -> std::vector<game::ScriptLoader>
    {
        auto custom_level_loaders = std::vector<game::ScriptLoader>{};

        if (std::filesystem::exists("levels"))
        {
            auto files = std::filesystem::directory_iterator{"levels"} | std::ranges::to<std::vector>();
            std::ranges::sort(files, [](const auto &a, const auto &b)
                              { return a.path() <= b.path(); });

            custom_level_loaders =
                files |
                std::views::filter([](const auto &e)
                                   { return e.path().extension() == ".lua"; }) |
                std::views::transform([](const auto &e)
                                      { return e.path().string(); }) |
                std::views::transform([](const auto &s)
                                      { return game::ScriptLoader{s}; }) |
                std::ranges::to<std::vector>();
        }

        const auto builtin_level_names =
            reader |
            std::views::filter([](const auto &entry)
                               {
            if (entry.type() == game::TlvType::TEXT_FILE)
            {
                const auto text_file = entry.text_file_value();
                return text_file.name.ends_with(".lua");
            }
            return false; }) |
            std::views::transform([&reader](const auto &entry)
                                  { return game::ScriptLoader{entry.text_file_value().name, reader}; }) |
            std::ranges::to<std::vector>();

        auto level_loaders = std::vector<game::ScriptLoader>{};
        level_loaders.insert_range(std::ranges::end(level_loaders), custom_level_loaders);
        level_loaders.insert_range(std::ranges::end(level_loaders), builtin_level_names);

        return level_loaders;
    }
}

namespace game::routines
{
    LevelRoutine::LevelRoutine(const Window &window, messaging::MessageBus &bus, Scheduler &scheduler, DefaultCache &resource_cache, const TlvReader &reader)
        : _window{window},
          _bus{bus},
          _scheduler{scheduler},
          _player{bus, create_camera(window)},
          _level_num{},
          _level_names{get_level_loaders(reader)},
          _resource_cache{resource_cache},
          _reader{reader},
          _level{std::make_unique<levels::LuaLevel>(_level_names[_level_num], _resource_cache, _reader, _player, _bus)},
          _show_physics_debug{false},
          _show_debug{false},
          _running{true},
          _auto_subscribe{bus, {messaging::MessageType::LEVEL_COMPLETE, messaging::MessageType::QUIT}, this}
    {
        _window.set_title(_level_names[_level_num].name());
    }

    auto LevelRoutine::create_task() -> Task
    {
        auto curernt_level = _level_num;
        while (_running)
        {
            if (_level == nullptr || curernt_level != _level_num)
            {
                _player.restart();
                _level.reset();
                _level = std::make_unique<levels::LuaLevel>(_level_names[_level_num], _resource_cache, _reader, _player, _bus);
                _level->restart();
                curernt_level = _level_num;

                _window.set_title(_level_names[_level_num].name());
            }
            auto *level = _level.get();

            _player.update();
            level->update(_player);

            for (auto &entity : level->scene().entities)
            {
                entity->set_visibility(intersects_frustum(entity->bounding_box(), _player.camera().frustum_planes()));

                entity->bounding_box().draw(level->physics().debug_renderer());
            }

            if (_running)
            {
                co_await Wait{_scheduler, 1u};
            }
        }
    }
    auto LevelRoutine::player() const -> const Player &
    {
        return _player;
    }

    auto LevelRoutine::level() const -> levels::LuaLevel *
    {
        return _level.get();
    }

    auto LevelRoutine::handle_level_complete(const std::string_view &level_name) -> void
    {
        game::log::info("level complete: {}", level_name);
        _level_num++;
        if (_level_num >= _level_names.size())
        {
            log::info("YOU WIN !");
            // _running = false;
            _level_num = 0;
        }
    }

    auto LevelRoutine::handle_key_press(const KeyEvent &event) -> void
    {
        if (event.key() == game::Key::F1 && event.state() == game::KeyState::UP)
        {
            _show_debug = !_show_debug;
            _player.set_flying(_show_debug);
            if (_level)
            {
                _level->set_show_debug(_show_debug);
            }
        }
        else if (event.key() == game::Key::F2 && event.state() == game::KeyState::UP)
        {
            _show_physics_debug = !_show_physics_debug;
        }
    }

    auto LevelRoutine::handle_quit() -> void
    {
        _running = false;
    }
}
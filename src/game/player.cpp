#include "game/player.h"

#include <numbers>
#include <queue>

#include "events/key_event.h"
#include "game/game_state.h"
#include "graphics/camera.h"
#include "math/vector3.h"
#include "messaging/message_bus.h"

namespace game
{
    Player::Player(messaging::MessageBus &bus, Camera camera)
        : _camera(std::move(camera)),
          _key_state{},
          _flying{false},
          _start_position{camera.position()},
          _auto_subscribe{bus,
                          {messaging::MessageType::KEY_PRESS,
                           messaging::MessageType::MOUSE_MOVE,
                           messaging::MessageType::RESTART_LEVEL,
                           messaging::MessageType::STATE_CHANGE},
                          this},
          _frozen{false},
          _dead{false}
    {
    }

    auto Player::handle_key_press(const KeyEvent &event) -> void
    {
        if (_frozen)
        {
            return;
        }
        _key_state[event.key()] = event.state() == game::KeyState::DOWN;

        if (event.key() == game::Key::F1 && event.state() == game::KeyState::UP)
        {
            _flying = !_flying;
        }
    }

    auto Player::handle_mouse_move(const MouseEvent &event) -> void
    {
        if (_frozen)
        {
            return;
        }
        static constexpr auto sensitivity = float{0.005f};
        _camera.adjust_pitch(event.delta_y() * sensitivity);
        _camera.adjust_yaw(-event.delta_x() * sensitivity);
    }
    auto Player::handle_restart_level() -> void
    {
        restart();
    }

    auto Player::handle_state_change(GameState state) -> void
    {
        _frozen = state != GameState::RUNNING;
    }

    auto Player::camera() const -> const Camera &
    {
        return _camera;
    }

    auto Player::position() const -> Vector3
    {
        return _camera.position();
    }

    auto Player::flying() -> bool
    {
        return _flying;
    }

    auto Player::set_flying(bool flying) -> void
    {
        _flying = flying;
    }

    auto Player::update() -> void
    {
        auto walk_direction = game::Vector3{0.f};

        if (_key_state[game::Key::D] || _key_state[game::Key::RIGHT_ARROW])
        {
            walk_direction += _camera.right();
        }
        if (_key_state[game::Key::A] || _key_state[game::Key::LEFT_ARROW])
        {
            walk_direction -= _camera.right();
        }
        if (_key_state[game::Key::W] || _key_state[game::Key::UP_ARROW])
        {
            walk_direction += _camera.direction();
        }
        if (_key_state[game::Key::S] || _key_state[game::Key::DOWN_ARROW])
        {
            walk_direction -= _camera.direction();
        }
        if (_key_state[game::Key::SPACE])
        {
            walk_direction += _camera.up();
        }
        if (_key_state[game::Key::LCTRL])
        {
            walk_direction -= _camera.up();
        }

        if (!_flying)
        {
            walk_direction.y = 0.f;
        }

        const auto speed = _key_state[game::Key::LSHIFT] ? 30.f : 10.f;
        _camera.translate(game::Vector3::normalize(walk_direction) * (speed / 60.f));
        _camera.update();
    }

    auto Player::restart() -> void
    {
        _camera.set_position(_start_position);
        _camera.set_pitch(0.f);
        _camera.set_yaw(-std::numbers::pi_v<float> / 2.f);

        _camera.update();
    }
}
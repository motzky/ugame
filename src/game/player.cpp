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
    Player::Player(messaging::MessageBus &bus, Camera camera, CharacterController &controller)
        : _camera(std::move(camera)),
          _key_state{},
          _flying{false},
          _start_position{camera.position()},
          _controller{controller},
          _auto_subscribe{bus,
                          {messaging::MessageType::KEY_PRESS,
                           messaging::MessageType::MOUSE_MOVE,
                           messaging::MessageType::RESTART_LEVEL,
                           messaging::MessageType::STATE_CHANGE},
                          this},
          _frozen{false},
          _dead{false}
    {
        _controller.set_position(_start_position);
    }

    auto Player::handle_key_press(const KeyEvent &event) -> void
    {
        if (_frozen)
        {
            return;
        }
        _key_state[event.key()] = event.state() == KeyState::DOWN;

        if (event.key() == Key::F1 && event.state() == KeyState::UP)
        {
            _flying = !_flying;
            log::debug("player flying chaged to {}", _flying);
            if (!_flying)
            {
                _camera.set_position(_controller.position());
                _camera.update();
            }
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

    auto Player::update() -> void
    {
        auto walk_direction = Vector3{0.f};

        if (_key_state[Key::D] || _key_state[Key::RIGHT_ARROW])
        {
            walk_direction += _camera.right();
        }
        if (_key_state[Key::A] || _key_state[Key::LEFT_ARROW])
        {
            walk_direction -= _camera.right();
        }
        if (_key_state[Key::W] || _key_state[Key::UP_ARROW])
        {
            walk_direction += _camera.direction();
        }
        if (_key_state[Key::S] || _key_state[Key::DOWN_ARROW])
        {
            walk_direction -= _camera.direction();
        }
        if (_key_state[Key::SPACE])
        {
            walk_direction += _camera.up();
        }
        if (_key_state[Key::LCTRL])
        {
            walk_direction -= _camera.up();
        }

        if (!_flying)
        {
            walk_direction.y = 0.f;
        }

        const auto speed = _key_state[Key::LSHIFT] ? 30.f : 10.f;
        const auto velocity = Vector3::normalize(walk_direction) * (speed / 60.f);
        if (!_flying)
        {
            _controller.set_linear_velocity(velocity * 180.f);
            _camera.set_position(_controller.position());
        }
        else
        {
            _camera.translate(velocity);
        }
        _camera.update();
    }

    auto Player::restart() -> void
    {
        _flying = false;
        _camera.set_position(_start_position);
        _controller.set_position(_start_position);
        _camera.set_pitch(0.f);
        _camera.set_yaw(-std::numbers::pi_v<float> / 2.f);

        _camera.update();
    }
}
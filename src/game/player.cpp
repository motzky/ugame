#include "game/player.h"

#include <queue>

#include "camera.h"
#include "events/key_event.h"
#include "math/vector3.h"
#include "messaging/message_bus.h"

namespace game
{
    Player::Player(messaging::MessageBus &bus, Camera camera)
        : _camera(std::move(camera)),
          _key_state{},
          _flying{false}
    {
        bus.subscribe(messaging::MessageType::KEY_PRESS, this);
        bus.subscribe(messaging::MessageType::MOUSE_MOVE, this);
    }

    auto Player::handle_key_press(const KeyEvent &event) -> void
    {
        _key_state[event.key()] = event.state() == game::KeyState::DOWN;
    }

    auto Player::handle_mouse_move(const MouseEvent &event) -> void
    {
        static constexpr auto sensitivity = float{0.005f};
        _camera.adjust_pitch(event.delta_y() * sensitivity);
        _camera.adjust_yaw(-event.delta_x() * sensitivity);
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
}
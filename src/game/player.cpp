#include "game/player.h"

#include "camera.h"
#include "events/key_event.h"
#include "log.h"
#include "math/vector3.h"
#include "messaging/message_bus.h"

namespace game
{
    Player::Player(messaging::MessageBus &bus, Camera camera)
        : _camera(std::move(camera))
    {
        bus.subscribe(messaging::MessageType::KEY_PRESS, this);
    }

    auto Player::handle_key_press(const KeyEvent &event) -> void
    {
        log::debug("{}", event);
    }

    auto Player::camera() const -> const Camera &
    {
        return _camera;
    }

    auto Player::position() const -> Vector3
    {
        return _camera.position();
    }

}
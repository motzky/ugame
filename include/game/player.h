#pragma once

#include <queue>
#include <unordered_map>

#include "camera.h"
#include "events/key_event.h"
#include "events/mouse_event.h"
#include "math/vector3.h"
#include "messaging/message_bus.h"
#include "messaging/subscriber.h"

namespace game
{
    class Player : public messaging::Subscriber
    {
    public:
        Player(messaging::MessageBus &bus, Camera camera);

        auto handle_key_press(const KeyEvent &event) -> void override;
        auto handle_mouse_move(const MouseEvent &event) -> void override;

        auto camera() const -> const Camera &;
        auto position() const -> Vector3;
        auto flying() -> bool;

        auto set_flying(bool flying) -> void;

        auto update() -> void;

    private:
        Camera _camera;
        std::unordered_map<game::Key, bool> _key_state;
        std::queue<game::KeyEvent> _key_event_buffer;
        bool _flying;
    };
}
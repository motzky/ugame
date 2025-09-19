#pragma once

#include "camera.h"
#include "events/key_event.h"
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

        auto camera() const -> const Camera &;
        auto position() const -> Vector3;

    private:
        Camera _camera;
    };
}
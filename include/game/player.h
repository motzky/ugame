#pragma once

#include <queue>
#include <unordered_map>

#include "events/key_event.h"
#include "events/mouse_event.h"
#include "game/game_state.h"
#include "graphics/camera.h"
#include "math/vector3.h"
#include "messaging/auto_subscribe.h"
#include "messaging/message_bus.h"
#include "messaging/subscriber.h"
#include "physics/character_controller.h"

namespace game
{
    class Player : public messaging::Subscriber
    {
    public:
        Player(messaging::MessageBus &bus, Camera camera, CharacterController &controller);

        auto handle_key_press(const KeyEvent &event) -> void override;
        auto handle_mouse_move(const MouseEvent &event) -> void override;
        auto handle_restart_level() -> void override;
        auto handle_state_change(GameState state) -> void override;

        auto camera() const -> const Camera &;
        auto position() const -> Vector3;
        auto flying() -> bool;

        auto set_flying(bool flying) -> void;

        auto update() -> void;

        auto restart() -> void;

    private:
        Camera _camera;
        std::unordered_map<game::Key, bool> _key_state;
        bool _flying;
        Vector3 _start_position;
        CharacterController &_controller;
        messaging::AutoSubscribe _auto_subscribe;
        bool _frozen;
        bool _dead;
    };
}
#pragma once

#include "game/game_state.h"
#include "messaging/message_bus.h"
#include "messaging/subscriber.h"

namespace game
{
    class RoutineBase : public messaging::Subscriber
    {
    public:
        RoutineBase(messaging::MessageBus &bus);
        virtual ~RoutineBase() = default;

        virtual auto handle_state_change(GameState state) -> void override;

    protected:
        messaging::MessageBus &_bus;
        GameState _state;
    };
}
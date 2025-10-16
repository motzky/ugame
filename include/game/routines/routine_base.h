#pragma once

#include <set>

#include "game/game_state.h"
#include "messaging/auto_subscribe.h"
#include "messaging/message_bus.h"
#include "messaging/subscriber.h"

namespace game
{
    class RoutineBase : public messaging::Subscriber
    {
    public:
        RoutineBase(messaging::MessageBus &bus, std::set<messaging::MessageType> types);
        virtual ~RoutineBase() = default;

        virtual auto handle_state_change(GameState state) -> void override;

    protected:
        messaging::MessageBus &_bus;
        GameState _state;

    private:
        messaging::AutoSubscribe _auto_subscribe;
    };
}
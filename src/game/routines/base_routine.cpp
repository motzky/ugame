#include "game/routines/base_routine.h"

#include "game/game_state.h"
#include "messaging/message_bus.h"
#include "messaging/subscriber.h"

namespace game
{
    RoutineBase::RoutineBase(messaging::MessageBus &bus)
        : _bus{bus}, _state{GameState::RUNNING}
    {
        _bus.subscribe(messaging::MessageType::STATE_CHANGE, this);
    }

    auto RoutineBase::handle_state_change(GameState state) -> void
    {
        _state = state;
    }

}
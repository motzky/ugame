#include "game/routines/base_routine.h"

namespace game
{
    RoutineBase::RoutineBase(messaging::MessageBus &bus)
        : _bus{bus}, _state{}
    {
    }

    auto RoutineBase::handle_state_change(GameState state) -> void
    {
        _state = state;
    }

}
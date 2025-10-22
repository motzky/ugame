#include "game/routines/routine_base.h"

#include <set>

#include "game/game_state.h"
#include "messaging/message_bus.h"
#include "messaging/subscriber.h"

namespace
{
    auto enrich_types(std::set<game::messaging::MessageType> types) -> std::set<game::messaging::MessageType>
    {
        types.emplace(game::messaging::MessageType::STATE_CHANGE);
        return types;
    }
}

namespace game
{
    RoutineBase::RoutineBase(messaging::MessageBus &bus, std::set<messaging::MessageType> types)
        : _bus{bus},
          _state{GameState::MAIN_MENU},
          _auto_subscribe{_bus, enrich_types(std::move(types)), this}
    {
    }

    auto RoutineBase::handle_state_change(GameState state) -> void
    {
        _state = state;
    }

}
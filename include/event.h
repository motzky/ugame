#pragma once

#include <variant>

#include "stop_event.h"
#include "key_event.h"

namespace game
{
    using Event = std::variant<StopEvent, KeyEvent>;
}
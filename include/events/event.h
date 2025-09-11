#pragma once

#include <variant>

#include "events/key_event.h"
#include "events/mouse_event.h"
#include "mouse_button_event.h"
#include "stop_event.h"

namespace game
{
    using Event = std::variant<StopEvent, KeyEvent, MouseEvent, MouseButtonEvent>;
}
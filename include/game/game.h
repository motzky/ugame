#pragma once

#include <cstddef>
#include <string_view>
#include <vector>

#include "messaging/message_bus.h"
#include "messaging/subscriber.h"
#include "window.h"

namespace game
{

    class Game : public messaging::Subscriber
    {
    public:
        Game(const std::vector<std::string_view> &args);
        ~Game();

        auto run(std::string_view resource_root) -> void;

    private:
        bool _running;
        messaging::MessageBus _message_bus;
        std::uint8_t _samples;
        Window _window;
    };
}

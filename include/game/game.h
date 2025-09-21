#pragma once

#include <string_view>

#include "messaging/message_bus.h"
#include "messaging/subscriber.h"

namespace game
{
    class Game : public messaging::Subscriber
    {
    public:
        Game();
        ~Game();

        auto run(std::string_view resource_root) -> void;

        virtual auto handle_level_complete(const std::string_view &) -> void override;

    private:
        bool _running;
        std::size_t _level_num;
    };
}

#pragma once

#include <cstddef>
#include <memory>
#include <string_view>
#include <vector>

#include "game/levels/level.h"
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
        std::vector<std::unique_ptr<levels::Level>> _levels;

        std::size_t _level_num;
    };
}

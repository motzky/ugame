#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "game/levels/lua_level.h"
#include "game/player.h"
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

        virtual auto handle_level_complete(const std::string_view &) -> void override;

    private:
        bool _running;
        std::vector<std::string> _level_names;

        std::unique_ptr<levels::LuaLevel> _level;
        std::size_t _level_num;
        messaging::MessageBus _message_bus;
        Window _window;
        Player _player;
    };
}

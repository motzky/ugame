#include "messaging/message_bus.h"

#include <algorithm>
#include <string_view>

#include "events/key_event.h"
#include "events/mouse_button_event.h"
#include "events/mouse_event.h"
#include "messaging/subscriber.h"
#include "utils/ensure.h"

namespace
{

    template <class... Args>
    auto post_message(game::messaging::MessageType type, auto &subscribers, auto func, Args &&...args)
    {
        for (auto *subscriber : subscribers[type])
        {
            func(subscriber, std::forward<Args>(args)...);
        }
    }
}

namespace game::messaging
{
    auto MessageBus::subscribe(MessageType type, Subscriber *subscriber) -> void
    {
        auto &subscribers = _subscribers[type];
        expect(!std::ranges::contains(subscribers, subscriber), "subscriber already subscribed");
        subscribers.push_back(subscriber);
    }

    auto MessageBus::post_key_press(const KeyEvent &event) -> void
    {
        post_message(MessageType::KEY_PRESS, _subscribers, [](auto *sub, const KeyEvent &evt)
                     { sub->handle_key_press(evt); }, event);
    }

    auto MessageBus::post_mouse_move(const MouseEvent &event) -> void
    {
        post_message(MessageType::MOUSE_MOVE, _subscribers, [](auto *sub, const MouseEvent &evt)
                     { sub->handle_mouse_move(evt); }, event);
    }

    auto MessageBus::post_mouse_button(const MouseButtonEvent &event) -> void
    {
        post_message(MessageType::MOUSE_BUTTON_PRESS, _subscribers, [](auto *sub, const MouseButtonEvent &evt)
                     { sub->handle_mouse_button(evt); }, event);
    }

    auto MessageBus::post_level_complete(const std::string_view &level_name) -> void
    {
        post_message(MessageType::LEVEL_COMPLETE, _subscribers, [](auto *sub, const auto &level_name)
                     { sub->handle_level_complete(level_name); }, level_name);
    }
}

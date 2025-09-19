#include "messaging/message_bus.h"

#include <algorithm>

#include "ensure.h"
#include "events/key_event.h"
#include "events/mouse_button_event.h"
#include "events/mouse_event.h"
#include "messaging/subscriber.h"

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
        for (auto *subscriber : _subscribers[MessageType::KEY_PRESS])
        {
            subscriber->handle_key_press(event);
        }
    }

    auto MessageBus::post_mouse_move(const MouseEvent &event) -> void
    {
        for (auto *subscriber : _subscribers[MessageType::MOUSE_MOVE])
        {
            subscriber->handle_mouse_move(event);
        }
    }

    auto MessageBus::post_mouse_button(const MouseButtonEvent &event) -> void
    {
        for (auto *subscriber : _subscribers[MessageType::MOUSE_BUTTON_PRESS])
        {
            subscriber->handle_mouse_button(event);
        }
    }
}
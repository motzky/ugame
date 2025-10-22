#include "messaging/message_bus.h"

#include <algorithm>
#include <string_view>

#include "events/key_event.h"
#include "events/mouse_button_event.h"
#include "events/mouse_event.h"
#include "messaging/subscriber.h"
#include "primitives/entity.h"
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

    auto MessageBus::unsubscribe(MessageType type, Subscriber *subscriber) -> void
    {
        auto &subscribers = _subscribers[type];
        expect(std::ranges::contains(subscribers, subscriber), "subscriber not subscribed");
        std::erase(subscribers, subscriber);
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

    auto MessageBus::post_entity_intersect(const Entity *a, const Entity *b) -> void
    {
        post_message(MessageType::ENTITY_INTERSECT, _subscribers, [](auto *sub, const auto *a, const auto *b)
                     { sub->handle_entity_intersect(a, b); }, a, b);
    }

    auto MessageBus::post_restart_level() -> void
    {
        post_message(MessageType::RESTART_LEVEL, _subscribers, [](auto *sub)
                     { sub->handle_restart_level(); });
    }

    auto MessageBus::post_quit() -> void
    {
        post_message(MessageType::QUIT, _subscribers, [](auto *sub)
                     { sub->handle_quit(); });
    }

    auto MessageBus::post_state_change(GameState state) -> void
    {
        post_message(MessageType::STATE_CHANGE, _subscribers, [state](auto *sub)
                     { sub->handle_state_change(state); });
    }

    auto MessageBus::post_change_camera(const game::Camera *camera) -> void
    {
        post_message(MessageType::CHANGE_CAMERA, _subscribers, [](auto *sub, auto *c)
                     { sub->handle_change_camera(c); }, camera);
    }
    auto MessageBus::post_change_scene(game::Scene *scene) -> void
    {
        post_message(MessageType::CHANGE_SCENE, _subscribers, [](auto *sub, auto *s)
                     { sub->handle_change_scene(s); }, scene);
    }
}

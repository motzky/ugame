#include <gtest/gtest.h>

#include <optional>

#include "events/key_event.h"
#include "messaging/auto_subscribe.h"
#include "messaging/message_bus.h"
#include "messaging/subscriber.h"

struct TestSub : public game::messaging::Subscriber
{
    auto handle_key_press(const game::KeyEvent &event) -> void override
    {
        key_event = event;
    }

    std::optional<game::KeyEvent> key_event;
};

TEST(message_bus, post_key_mesage)
{
    auto msg_bus = game::messaging::MessageBus{};
    auto sub = TestSub{};

    msg_bus.subscribe(game::messaging::MessageType::KEY_PRESS, &sub);
    const auto event = game::KeyEvent{game::Key::A, game::KeyState::DOWN};

    msg_bus.post_key_press(event);

    EXPECT_TRUE(!!sub.key_event);
    ASSERT_EQ(*sub.key_event, event);
}

TEST(message_bus, subscribe_twice_dies)
{
    auto msg_bus = game::messaging::MessageBus{};
    auto sub = TestSub{};

    msg_bus.subscribe(game::messaging::MessageType::KEY_PRESS, &sub);
    EXPECT_DEATH(msg_bus.subscribe(game::messaging::MessageType::KEY_PRESS, &sub), "");
}

TEST(message_bus, unsubscribe)
{
    auto msg_bus = game::messaging::MessageBus{};
    auto sub = TestSub{};

    msg_bus.subscribe(game::messaging::MessageType::KEY_PRESS, &sub);
    const auto event = game::KeyEvent{game::Key::A, game::KeyState::DOWN};

    msg_bus.post_key_press(event);

    EXPECT_TRUE(!!sub.key_event);
    ASSERT_EQ(*sub.key_event, event);

    sub.key_event = std::nullopt;

    msg_bus.unsubscribe(game::messaging::MessageType::KEY_PRESS, &sub);
    msg_bus.post_key_press(event);

    EXPECT_FALSE(!!sub.key_event);
}

TEST(message_bus, unsubscribe_dies_when_not_subscribed)
{
    auto msg_bus = game::messaging::MessageBus{};
    auto sub = TestSub{};

    EXPECT_DEATH(msg_bus.unsubscribe(game::messaging::MessageType::KEY_PRESS, &sub), "");
}

TEST(message_bus, auto_subscribe)
{
    auto msg_bus = game::messaging::MessageBus{};
    auto sub = TestSub{};
    const auto event = game::KeyEvent{game::Key::A, game::KeyState::DOWN};

    {
        auto auto_sub = game::messaging::AutoSubscribe{msg_bus, {game::messaging::MessageType::KEY_PRESS}, &sub};
        msg_bus.post_key_press(event);

        EXPECT_TRUE(!!sub.key_event);
        ASSERT_EQ(*sub.key_event, event);
    }

    sub.key_event = std::nullopt;

    msg_bus.post_key_press(event);

    EXPECT_FALSE(!!sub.key_event);
}
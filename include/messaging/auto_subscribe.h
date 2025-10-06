#pragma once

#include <set>

#include "messaging/message_bus.h"
#include "messaging/subscriber.h"

namespace game::messaging
{
    class AutoSubscribe
    {
    public:
        AutoSubscribe(MessageBus &bus, const std::set<MessageType> types, Subscriber *subscriber)
            : _bus(bus),
              _types(std::move(types)),
              _subscriber(subscriber)
        {
            for (const auto type : _types)
            {
                _bus.subscribe(type, _subscriber);
            }
        }
        ~AutoSubscribe()
        {
            for (const auto type : _types)
            {
                _bus.unsubscribe(type, _subscriber);
            }
        }

    private:
        MessageBus &_bus;
        std::set<MessageType> _types;
        Subscriber *_subscriber;
    };
}

#pragma once
#include <any>
#include <functional>
#include <queue>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <algorithm>

class EventBus
{
public:
    using SubscriptionID = std::uint64_t;

private:
    struct QueuedEvent
    {
        QueuedEvent(std::type_index type, std::any event)
            : type(type), event(std::move(event))
        {
        
        }

        std::type_index type;
        std::any event;
    };

    struct HandlerData
    {
        SubscriptionID id = 0;
        std::function<void(const std::any&)> callback;
    };

public:
    EventBus() = default;
    ~EventBus() = default;

    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;

    template<class EventType>
    SubscriptionID Subscribe(std::function<void(const EventType&)> callback);

    void Unsubscribe(SubscriptionID id);

    template<class EventType>
    void Publish(const EventType& event);

    void Dispatch();

    void Clear();

private:
    std::unordered_map<std::type_index, std::vector<HandlerData>> handlers_;
    std::queue<QueuedEvent> eventQueue_;

    SubscriptionID nextSubscriptionID_ = 1;
};

template<class EventType>
EventBus::SubscriptionID EventBus::Subscribe(std::function<void(const EventType&)> callback)
{
    const std::type_index type = std::type_index(typeid(EventType));

    const SubscriptionID id = nextSubscriptionID_++;

    HandlerData handler{};
    handler.id = id;

    handler.callback = [callback](const std::any& event){
        callback(std::any_cast<const EventType&>(event));
        };

    handlers_[type].push_back(std::move(handler));

    return id;
}

template<class EventType>
void EventBus::Publish(const EventType& event)
{
    QueuedEvent queuedEvent(
        std::type_index(typeid(EventType)),
        event
    );

    eventQueue_.push(std::move(queuedEvent));
}


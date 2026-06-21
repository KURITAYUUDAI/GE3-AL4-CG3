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

    template<class EventType, class Callback>
    SubscriptionID Subscribe(Callback&& callback);

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

template<class EventType, class Callback>
EventBus::SubscriptionID EventBus::Subscribe(Callback&& callback)
{
    const std::type_index type = std::type_index(typeid(EventType));

    const SubscriptionID id = nextSubscriptionID_++;

    HandlerData handler{};
    handler.id = id;

    handler.callback =
        [callback = std::forward<Callback>(callback)](const std::any& event){
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

class EventSubscriber
{
public:
    EventSubscriber() = default;
    ~EventSubscriber()
    {
        Finalize();
    }

    EventSubscriber(const EventSubscriber&) = delete;
    EventSubscriber& operator=(const EventSubscriber&) = delete;

    void Initialize(EventBus* eventBus)
    {
        eventBus_ = eventBus;
    }

    void Finalize()
    {
        UnregisterEvents();
        eventBus_ = nullptr;
    }

public:

    template<class EventType, class Callback>
    void Subscribe(Callback&& callback)
    {
        if (!eventBus_)
        {
            return;
        }

        EventBus::SubscriptionID id =
            eventBus_->Subscribe<EventType>(
                std::forward<Callback>(callback)
            );

        subscriptionIDs_.push_back(id);
    }

    void UnregisterEvents()
    {
        if (!eventBus_)
        {
            return;
        }

        for (EventBus::SubscriptionID id : subscriptionIDs_)
        {
            eventBus_->Unsubscribe(id);
        }

        subscriptionIDs_.clear();
    }

public:

    EventBus* GetEventBus() { return eventBus_; }

private:
    EventBus* eventBus_ = nullptr;
    std::vector<EventBus::SubscriptionID> subscriptionIDs_;
};

#include "EventBus.h"

void EventBus::Unsubscribe(SubscriptionID id)
{
    for (auto& [type, handlerList] : handlers_)
    {
        handlerList.erase(
            std::remove_if(
                handlerList.begin(),
                handlerList.end(),
                [id](const HandlerData& handler){
                    return handler.id == id;
                }),
            handlerList.end()
        );
    }
}

void EventBus::Dispatch()
{
    while (!eventQueue_.empty())
    {
        QueuedEvent queuedEvent = std::move(eventQueue_.front());
        eventQueue_.pop();

        auto it = handlers_.find(queuedEvent.type);
        if (it == handlers_.end())
        {
            continue;
        }

        auto& handlerList = it->second;

        for (auto& handler : handlerList)
        {
            handler.callback(queuedEvent.event);
        }
    }
}

void EventBus::Clear()
{
    while (!eventQueue_.empty())
    {
        eventQueue_.pop();
    }

    handlers_.clear();
}

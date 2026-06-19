#include "UIControllerBase.h"

void UIControllerBase::Initialize(EventBus* eventBus)
{
    eventBus_ = eventBus;
    RegisterEvents();
}

void UIControllerBase::Finalize()
{
    UnregisterEvents();
    eventBus_ = nullptr;
}

void UIControllerBase::AddSubscription(EventBus::SubscriptionID id)
{
    subscriptionIDs_.push_back(id);
}

void UIControllerBase::UnregisterEvents()
{
    if (!eventBus_)
    {
        return;
    }

    for (auto id : subscriptionIDs_)
    {
        eventBus_->Unsubscribe(id);
    }

    subscriptionIDs_.clear();
}

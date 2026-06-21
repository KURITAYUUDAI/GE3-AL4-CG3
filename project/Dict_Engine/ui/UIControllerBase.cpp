#include "UIControllerBase.h"

void UIControllerBase::Initialize(EventBus* eventBus)
{
    eventSubscriber_.Initialize(eventBus);
    RegisterEvents();
}

void UIControllerBase::Finalize()
{
    eventSubscriber_.Finalize();
}

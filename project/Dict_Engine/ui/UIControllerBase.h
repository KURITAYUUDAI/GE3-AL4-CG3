#pragma once
#include <vector>
#include "EventBus.h"

class UIControllerBase
{
public:
    virtual ~UIControllerBase() = default;

    virtual void Initialize(EventBus* eventBus);

    virtual void Finalize();

    virtual void Update(float deltaTime) = 0;

protected:
    virtual void RegisterEvents() = 0;

protected:
    EventSubscriber eventSubscriber_;
};
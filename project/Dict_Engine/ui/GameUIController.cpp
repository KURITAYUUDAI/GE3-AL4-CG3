#include "GameUIController.h"

void GameUIController::Initialize(EventBus* eventBus)
{
	eventBus_ = eventBus;

	RegisterEvents();
}

void GameUIController::Finalize()
{
	UnregisterEvents();

	eventBus_ = nullptr;
}

void GameUIController::Update(float deltaTime)
{
    deltaTime;
}

void GameUIController::RegisterEvents()
{
    if (!eventBus_)
    {
        return;
    }

    subscriptionIDs_.push_back(
        eventBus_->Subscribe<PlayerHPChangeEvent>(
            [this](const PlayerHPChangeEvent& event)
            {
                OnPlayerHPChanged(event);
            }
        )
    );
}

void GameUIController::UnregisterEvents()
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

void GameUIController::OnPlayerHPChanged(const PlayerHPChangeEvent& event)
{
	viewModel_.playerHitPoint.currentHitPoint = event.currentHitPoint;
    viewModel_.playerHitPoint.previousHitPoint = event.previousHitPoint;
	viewModel_.playerHitPoint.maxHitPoint = event.maxHitPoint;
}

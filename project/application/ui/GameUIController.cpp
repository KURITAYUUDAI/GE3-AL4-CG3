#include "GameUIController.h"


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

void GameUIController::OnPlayerHPChanged(const PlayerHPChangeEvent& event)
{
	viewModel_.playerHitPoint.currentHitPoint = event.currentHitPoint;
    viewModel_.playerHitPoint.previousHitPoint = event.previousHitPoint;
	viewModel_.playerHitPoint.maxHitPoint = event.maxHitPoint;
}

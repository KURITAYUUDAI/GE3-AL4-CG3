#pragma once
#include <vector>
#include "EventBus.h"
#include "UIUtility.h"
#include "player/PlayerEvent.h"

class GameUIController
{
public:
    void Initialize(EventBus* eventBus);
    void Finalize();

    void Update(float deltaTime);

    const GameUIViewModel& GetViewModel() const { return viewModel_; }

private:
    void RegisterEvents();
    void UnregisterEvents();

    void OnPlayerHPChanged(const PlayerHPChangeEvent& event);

private:
    EventBus* eventBus_ = nullptr;
    std::vector<EventBus::SubscriptionID> subscriptionIDs_;

    GameUIViewModel viewModel_;
};

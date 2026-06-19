#pragma once
#include "UIControllerBase.h"
#include <vector>
#include "EventBus.h"
#include "UIUtility.h"
#include "GamePlayUIUtility.h"
#include "player/PlayerEvent.h"

class GameUIController : public UIControllerBase
{
public:
   
    void Update(float deltaTime) override;

    const GameUIViewModel& GetViewModel() const { return viewModel_; }

private:
    void RegisterEvents() override;
   
    void OnPlayerHPChanged(const PlayerHPChangeEvent& event);

private:

    GameUIViewModel viewModel_;
};

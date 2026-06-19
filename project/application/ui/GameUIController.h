#pragma once
#include "UIControllerBase.h"
#include <vector>
#include "EventBus.h"
#include "UIUtility.h"
#include "GamePlayUIUtility.h"
#include "player/PlayerEvent.h"
#include "enemy/EnemyEvent.h"

class GameUIController : public UIControllerBase
{
public:
   
    void Update(float deltaTime) override;

    const GameUIViewModel& GetViewModel() const { return viewModel_; }

private:
    void RegisterEvents() override;
   
    void OnPlayerHPChanged(const PlayerHPChangeEvent& event);
    void OnEnemyAppier(const EnemyAppierEvent& event);
    void OnEnemyScreenPosition(const EnemyScreenPositionEvent& event);
    void OnEnemyHPChanged(const EnemyHPChangeEvent& event);

private:

    GameUIViewModel viewModel_;
};

#include "GamePlaySceneUI.h"
#include "HPGageUI.h"

void GamePlaySceneUI::Initialize(EventBus* eventBus)
{
    uiManager_ = std::make_unique<UIManager>();
    uiManager_->Initialize();

    gameUIController_ = std::make_unique<GameUIController>();
    gameUIController_->Initialize(eventBus);

    auto hpGageUI = std::make_unique<HPGageUI>();
    hpGageUI->Initialize();

    uiManager_->AddUI(std::move(hpGageUI));
}

void GamePlaySceneUI::Finalize()
{
    if (gameUIController_)
    {
        gameUIController_->Finalize();
        gameUIController_.reset();
    }

    uiManager_.reset();
}

void GamePlaySceneUI::Update(float deltaTime)
{
    gameUIController_->Update(deltaTime);

    uiManager_->Update(gameUIController_->GetViewModel(), deltaTime);
}

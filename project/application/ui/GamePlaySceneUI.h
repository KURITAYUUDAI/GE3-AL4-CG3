#pragma once
#include <memory>
#include "SceneUIBase.h"
#include "GameUIController.h"
#include "GamePlayUIUtility.h"

class GamePlaySceneUI : public SceneUIBase
{
public:
    void Initialize(EventBus* eventBus) override;
    void Finalize() override;
    void Update(float deltaTime) override;
    void DrawLayer(const UILayer& layer) override;

    void SetIsVisible(bool visible) override { uiManager_->SetIsVisible(visible); }

private:

    std::unique_ptr<UIManager<GameUIViewModel>> uiManager_;
    std::unique_ptr<GameUIController> gameUIController_;
};


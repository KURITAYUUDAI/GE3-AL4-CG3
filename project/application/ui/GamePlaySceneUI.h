#pragma once
#include <memory>
#include "SceneUIBase.h"
#include "GameUIController.h"

class GamePlaySceneUI : public SceneUIBase
{
public:
    void Initialize(EventBus* eventBus) override;
    void Finalize() override;

    void Update(float deltaTime) override;



private:
    std::unique_ptr<GameUIController> gameUIController_;
};


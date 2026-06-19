#pragma once
#include <memory>
#include "UIUtility.h"
#include "UIManager.h"
#include "EventBus.h"

class SceneUIBase
{
public:
	virtual ~SceneUIBase() = default;

	virtual void Initialize(EventBus* eventBus) = 0;
	virtual void Finalize() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void DrawLayer(const UILayer& layer)
	{
		uiManager_->DrawLayer(layer);
	}

	void SetIsVisible(bool visible)
	{
		uiManager_->SetIsVisible(visible);
	}

protected:
	std::unique_ptr<UIManager> uiManager_;
};


#pragma once
#include "UIBase.h"
#include <list>
#include <memory>

class SceneUIManager
{
public:

	void Initialize();

	void Update(const GameUIViewModel& viewModel, const float& deltaTime);

	void DrawLayer(const UILayer& layer);

	void AddUI(std::unique_ptr<UIBase> uiBase);

	void SetIsVisible(bool visible) { isVisible_ = visible; }

	bool GetIsVisible() const { return isVisible_; }

private:

	bool isVisible_ = true;
	std::list<std::unique_ptr<UIBase>> uiList_;

};


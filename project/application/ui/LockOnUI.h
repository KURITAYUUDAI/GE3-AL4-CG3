#pragma once
#include <memory>
#include <functional>
#include "UIBase.h"
#include "GamePlayUIUtility.h"
#include "Sprite.h"

class LockOnUI : public UIBase<GameUIViewModel>
{
public:

	using PositionGetter = std::function<Vector2(const GameUIViewModel&)>;
	using VisibilityGetter = std::function<bool(const GameUIViewModel&)>;

	void Initialize() override;
	void Update(const GameUIViewModel& viewModel, const float& deltaTime) override;
	void Draw() override;

public: // 外部入出力

	void SetPositionGetter(PositionGetter getter);
	void SetVisibilityGetter(VisibilityGetter getter);

	UILayer GetLayer() const override { return layer; }


private:
	
	PositionGetter positionGetter_;
	VisibilityGetter visibilityGetter_;

	float hpRate_ = 1.0f;
	UILayer layer = UILayer::Screen;

	std::unique_ptr<Sprite> lockOnSprite_;
};


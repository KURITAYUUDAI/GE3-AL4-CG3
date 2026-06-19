#pragma once
#include "UIBase.h"
#include "GamePlayUIUtility.h"
#include "Sprite.h"

class HPGageUI : public UIBase<GameUIViewModel>
{
public:

	void Initialize() override;
	void Update(const GameUIViewModel& viewModel, const float& deltaTime) override;
	void Draw() override;

public: // 外部入出力

	UILayer GetLayer() const override { return UILayer::Screen; }

private:
	float hpRate_ = 1.0f;

	std::unique_ptr<Sprite> backSprite_;
	std::unique_ptr<Sprite> fillSprite_;
};


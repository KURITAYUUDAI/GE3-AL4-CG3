#pragma once
#include <memory>
#include <functional>
#include "UIBase.h"
#include "GamePlayUIUtility.h"
#include "Sprite.h"

enum class HPGageDirection
{
	LeftToRight,
	RightToLeft,
	TopToBottom,
	BottomToTop
};

struct HPGageUIConfig
{
	Vector2 backPos = { 32.0f, 32.0f };
	Vector2 fillPos = {32.0f, 32.0f};
	Vector2 backSize =  {300.0f, 32.0f };
	Vector2 fillSize = { 300.0f, 32.0f };

	Vector4 backColor = { 0.8f, 0.8f, 0.8f, 1.0f };
	Vector4 fillColor = { 0.0f, 1.0f, 1.0f, 1.0f };

	std::string backTexturePath = "white1x1.png";
	std::string fillTexturePath = "white1x1.png";

	HPGageDirection direction = HPGageDirection::LeftToRight;
	UILayer layer = UILayer::Screen;
};

class HPGageUI : public UIBase<GameUIViewModel>
{
public:

	using HPGetter = std::function<const HPViewModel& (const GameUIViewModel&)>;
	using PositionGetter = std::function<Vector2(const GameUIViewModel&)>;

	void Initialize() override;
	void Update(const GameUIViewModel& viewModel, const float& deltaTime) override;
	void Draw() override;

public: // 外部入出力

	void SetConfig(const HPGageUIConfig& config);
	void SetHPGetter(HPGetter getter);
	void SetPositionGetter(PositionGetter getter);

	UILayer GetLayer() const override { return config_.layer; }

private:
	void ApplySprites();
	void ApplyDirectionAnchor();
	void UpdateFillSprite();

private:
	HPGageUIConfig config_{};
	HPGetter hpGetter_;
	PositionGetter positionGetter_;

	float hpRate_ = 1.0f;

	std::unique_ptr<Sprite> backSprite_;
	std::unique_ptr<Sprite> fillSprite_;
};


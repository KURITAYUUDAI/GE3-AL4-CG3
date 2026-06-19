#include "HPGageUI.h"
#include "ImGuiManager.h"

void HPGageUI::Initialize()
{
	/*backSprite_ = std::make_unique<Sprite>();
	backSprite_->Initialize("white1x1.png");
	backSprite_->SetPosition({32, 32});
	backSprite_->SetSize({300, 32});
	backSprite_->SetColor({0.8f, 0.8f, 0.8f, 1.0f});;
	fillSprite_ = std::make_unique<Sprite>();
	fillSprite_->Initialize("white1x1.png");
	fillSprite_->SetPosition({ 32, 32 });
	fillSprite_->SetSize({ 300.0f, 32.0f });
	fillSprite_->SetColor({ 0.0f, 1.0f, 1.0f, 1.0f });*/

	backSprite_ = std::make_unique<Sprite>();
	backSprite_->Initialize(config_.backTexturePath);

	fillSprite_ = std::make_unique<Sprite>();
	fillSprite_->Initialize(config_.fillTexturePath);

	ApplySprites();

	hpRate_ = 1.0f;
}

void HPGageUI::Update(const GameUIViewModel & viewModel, const float& deltaTime)
{
	if (!hpGetter_)
	{
		return;
	}

#ifdef _DEBUG
	//ImGui::Begin("HPGageUI Setting");

	//int playerHP = viewModel.playerHitPoint.currentHitPoint;
	//ImGui::InputInt("playerHp", &playerHP, 0, 0, ImGuiInputTextFlags_ReadOnly);
	//ImGui::InputFloat("hpRate", &hpRate_);

	//ImGui::End();
#endif

	const HPViewModel& hp = hpGetter_(viewModel);

	if (hp.maxHitPoint <= 0)
	{
		hpRate_ = 0.0f;
	} else
	{
		hpRate_ =
			static_cast<float>(hp.currentHitPoint) /
			static_cast<float>(hp.maxHitPoint);
	}
	if (positionGetter_)
	{
		Vector2 position = positionGetter_(viewModel);
		config_.backPos = position;
		config_.fillPos = position;
		backSprite_->SetPosition(position);
		fillSprite_->SetPosition(position);
	#ifdef _DEBUG

		ImGui::Begin("enemyHPGage");
		ImGui::InputFloat2("hpGagePosition", &position.x);
		ImGui::End();
	#endif
	}


	hpRate_ = std::clamp(hpRate_, 0.0f, 1.0f);

	UpdateFillSprite();

	

	backSprite_->Update();
	fillSprite_->Update();
}

void HPGageUI::Draw()
{
	if (!isVisible_)
	{
		return;
	}

	backSprite_->Draw();
	fillSprite_->Draw();
}

void HPGageUI::SetConfig(const HPGageUIConfig& config)
{
	config_ = config;

	if (backSprite_ && fillSprite_)
	{
		ApplySprites();
	}
}

void HPGageUI::SetHPGetter(HPGetter getter)
{
	hpGetter_ = std::move(getter);
}

void HPGageUI::SetPositionGetter(PositionGetter getter)
{
	positionGetter_ = std::move(getter);
}

void HPGageUI::ApplySprites()
{
	/*backSprite_->SetTexture(config_.backTexturePath);*/
	backSprite_->SetPosition(config_.backPos);
	backSprite_->SetSize(config_.backSize);
	backSprite_->SetColor(config_.backColor);

	/*fillSprite_->SetTexture(config_.fillTexturePath);*/
	fillSprite_->SetColor(config_.fillColor);

	ApplyDirectionAnchor();
	UpdateFillSprite();

	backSprite_->Update();
	fillSprite_->Update();
}

void HPGageUI::ApplyDirectionAnchor()
{
	Vector2 anchor = { 0.0f, 0.0f };
	Vector2 position = config_.fillPos;

	switch (config_.direction)
	{
	case HPGageDirection::LeftToRight:
		anchor = { 0.0f, 0.0f };
		position = config_.fillPos;
		break;

	case HPGageDirection::RightToLeft:
		anchor = { 1.0f, 0.0f };
		position = {
			config_.fillPos.x,// + config_.fillSize.x,
			config_.fillPos.y
		};
		break;

	case HPGageDirection::TopToBottom:
		anchor = { 0.0f, 0.0f };
		position = config_.fillPos;
		break;

	case HPGageDirection::BottomToTop:
		anchor = { 0.0f, 1.0f };
		position = {
			config_.fillPos.x,
			config_.fillPos.y, //+ config_.fillSize.y
		};
		break;
	}

	backSprite_->SetAnchorPoint(anchor);
	fillSprite_->SetAnchorPoint(anchor);
	fillSprite_->SetPosition(position);
}

void HPGageUI::UpdateFillSprite()
{
	Vector2 size = config_.fillSize;

	switch (config_.direction)
	{
	case HPGageDirection::LeftToRight:
	case HPGageDirection::RightToLeft:
		size.x = config_.fillSize.x * hpRate_;
		break;

	case HPGageDirection::TopToBottom:
	case HPGageDirection::BottomToTop:
		size.y = config_.fillSize.y * hpRate_;
		break;
	}

	fillSprite_->SetSize(size);
}
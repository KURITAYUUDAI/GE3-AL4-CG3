#include "HPGageUI.h"
#include "ImGuiManager.h"

void HPGageUI::Initialize()
{
	backSprite_ = std::make_unique<Sprite>();
	backSprite_->Initialize("white1x1.png");
	backSprite_->SetPosition({32, 32});
	backSprite_->SetSize({300, 32});
	backSprite_->SetColor({0.8f, 0.8f, 0.8f, 1.0f});;
	fillSprite_ = std::make_unique<Sprite>();
	fillSprite_->Initialize("white1x1.png");
	fillSprite_->SetPosition({ 32, 32 });
	fillSprite_->SetSize({ 300.0f, 32.0f });
	fillSprite_->SetColor({ 0.0f, 1.0f, 1.0f, 1.0f });

	hpRate_ = 1.0f;
}

void HPGageUI::Update(const GameUIViewModel & viewModel, const float& deltaTime)
{
#ifdef _DEBUG
	ImGui::Begin("HPGageUI Setting");

	int playerHP = viewModel.playerHP;
	ImGui::InputInt("playerHp", &playerHP, 0, 0, ImGuiInputTextFlags_ReadOnly);
	ImGui::InputFloat("hpRate", &hpRate_);

	ImGui::End();
#endif

	hpRate_ = static_cast<float>(viewModel.playerHP) / static_cast<float>(viewModel.playerMaxHP);

	fillSprite_->SetSize({300.0f * hpRate_, {fillSprite_->GetSize().y}});

	backSprite_->Update();
	fillSprite_->Update();
}

void HPGageUI::Draw()
{
	backSprite_->Draw();
	fillSprite_->Draw();
}

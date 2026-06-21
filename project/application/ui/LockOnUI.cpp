#include "LockOnUI.h"

void LockOnUI::Initialize()
{
	lockOnSprite_ = std::make_unique<Sprite>();
	lockOnSprite_->Initialize("lockOn.png");
    lockOnSprite_->SetSize(Vector2{ 200.0f, 200.0f });
    lockOnSprite_->SetColor({1.0f, 1.0f, 0.0f, 1.0f});
    lockOnSprite_->SetAnchorPoint({ 0.5f, 0.5f });
    lockOnSprite_->SetBlendMode(PSOManager::BlendMode::Add);
    lockOnSprite_->Update();
}

void LockOnUI::Update(const GameUIViewModel & viewModel, const float& deltaTime)
{
    if (!positionGetter_ || !visibilityGetter_)
    {
        return;
    }

    bool isTargetVisible = visibilityGetter_(viewModel);
    if (!isTargetVisible)
    {
        // ターゲットが見えない、または存在しない場合は表示しない
        // ※ UIBase に isVisible_ が定義されている前提（HPGageUIで使われているため）
        isVisible_ = false;
        return;
    }
    isVisible_ = true;

    // 座標の更新
    Vector2 position = positionGetter_(viewModel);
    lockOnSprite_->SetPosition(position);

    lockOnSprite_->Update();
}

void LockOnUI::Draw()
{
    if (!isVisible_)
    {
        return;
    }

    lockOnSprite_->Draw();
}

void LockOnUI::SetPositionGetter(PositionGetter getter)
{
    positionGetter_ = std::move(getter);
}

void LockOnUI::SetVisibilityGetter(VisibilityGetter getter)
{
    visibilityGetter_ = std::move(getter);
}

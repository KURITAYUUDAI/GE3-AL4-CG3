#define NOMINMAX
#include "Fade.h"

using namespace KamataEngine;

void Fade::Initialize()
{

	textureHandle_ = TextureManager::Load("white1x1.png");
	sprite_ = Sprite::Create(textureHandle_, { 0.0f,0.0f });
	sprite_->SetSize(Vector2(1280.0f, 720.0f));
	sprite_->SetColor(Vector4(0, 0, 0, 1));

}

void Fade::Update()
{ 
	switch (status_) 
	{ 
	case Status::None:
		// 何もしない
		break;

	case Status::FadeIn:

		// 1フレーム分の秒数をカウントアップ
		counter_ += 1.0f / 60.0f;
		// フェード継続時間に達したら打ち止め
		counter_ = std::min(counter_, duration_);
		// 0.0fから1.0fの間で、経過時間がフェード継続時間に近づくほどアルファ値を大きくする
		sprite_->SetColor(Vector4(0, 0, 0, std::clamp(1.0f - (counter_ / duration_), 0.0f, 1.0f)));


		break;

	case Status::FadeOut:

		// 1フレーム分の秒数をカウントアップ
		counter_ += 1.0f / 60.0f;
		// フェード継続時間に達したら打ち止め
		counter_ = std::min(counter_, duration_);
		// 0.0fから1.0fの間で、経過時間がフェード継続時間に近づくほどアルファ値を大きくする
		sprite_->SetColor(Vector4(0, 0, 0, std::clamp(counter_ / duration_, 0.0f, 1.0f)));

		break;
	}

	/*if (IsFinished())
	{
		status_ = Status::None;
	}*/
}

void Fade::Draw()
{
	if (status_ == Status::None)
	{
		return;
	}

	Sprite::PreDraw(DirectXCommon::GetInstance()->GetCommandList());
	sprite_->Draw();
	Sprite::PostDraw();
}

void Fade::Start(Status status, float duration)
{
	status_ = status;
	duration_ = duration;
	counter_ = 0.0f;
}

void Fade::Stop()
{
	status_ = Status::None;
}

bool Fade::IsFinished() const
{
	// フェード状態による分岐
	switch (status_)
	{ 
	case Status::FadeIn:

		if (counter_ >= duration_) 
		{
			return true;
		} 
		else 
		{
			return false;
		}

		break;

	case Status::FadeOut:

		if (counter_ >= duration_)
		{
			return true;
		} 
		else
		{
			return false;
		}
	}

	return true;
}
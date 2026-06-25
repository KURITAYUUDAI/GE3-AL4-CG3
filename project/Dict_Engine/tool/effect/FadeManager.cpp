#define NOMINMAX
#include "FadeManager.h"

std::unique_ptr<FadeManager> FadeManager::instance_ = nullptr;

FadeManager* FadeManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = std::make_unique<FadeManager>(ConstructorKey());
	}
	return instance_.get();
}

void FadeManager::Finalize()
{
	instance_.reset();
}

void FadeManager::Initialize()
{
	/*postEffectManager_->RegisterFactory("Dissolve",
		[]{ return std::make_unique<Dissolve>(); });*/

	for (auto type : {
		FadeType::Dissolve })
	{
		FadeState state;
		state.active = false;
		state.duration = 0.0f;
		state.timer = 0.0f;
		state.status = Status::None;
		fadeStates_[type] = state;
	}
}

void FadeManager::Update()
{
	for (auto& fadeState : fadeStates_)
	{
		FadeState& state = fadeState.second;

		if (state.status == Status::None) continue;

		state.timer = std::min(state.timer + 1.0f / 60.0f, state.duration);

		switch (state.status)
		{
		case Status::FadeIn:
			// FadeIn: 1→0（エフェクトが薄くなっていく）
			state.progress = 1.0f - (state.timer / state.duration);
			break;

		case Status::FadeOut:
			// FadeOut: 0→1（エフェクトが濃くなっていく）
			state.progress = state.timer / state.duration;
			break;

		default:
			break;
		}
	}

	// 各エフェクトへ進行度を反映（Addされている場合のみ）
	dissolve_ = postEffectManager_->Get<Dissolve>("Dissolve");
	if (dissolve_)
	{
		dissolve_->SetThreshold(fadeStates_[FadeType::Dissolve].progress);
	}
}

void FadeManager::Start(FadeType fadeType, Status status, float duration)
{
	auto& state = fadeStates_[fadeType];
	state.status = status;
	state.duration = duration;
	state.timer = 0.0f;
	state.progress = (status == Status::FadeIn) ? 1.0f : 0.0f;
	state.active = true;

	// チェーンに未追加なら追加
	const std::string name = "Dissolve";
	if (!postEffectManager_->Get<PostEffect>(name))
	{
		postEffectManager_->Add(name);
	}
}

void FadeManager::Stop(FadeType fadeType)
{
	auto& state = fadeStates_[fadeType];
	state.status = Status::None;
	state.active = false;

	const std::string name = "Dissolve";
	if (postEffectManager_->Get<PostEffect>(name))
	{
		postEffectManager_->Remove(name);
	}
}

bool FadeManager::IsFinished(FadeType fadeType)
{
	auto& state = fadeStates_[fadeType];

	switch (state.status)
	{
	case Status::FadeIn:

		if (state.timer >= state.duration)
		{
			return true;
		} else
		{
			return false;
		}

		break;

	case Status::FadeOut:

		if (state.timer >= state.duration)
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

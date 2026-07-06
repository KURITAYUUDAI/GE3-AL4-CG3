#include "SceneTransition.h"
#include "time/DeltaTimeManager.h"
#include <algorithm>

void SceneTransition::Start(
	float fadeOutDuration, 
	float fadeInDuration, 
	std::function<void()> onSwitchScene, 
	std::unique_ptr<ITransitionTrack> track)
{
    if (state_ != SceneTransitionState::Idle)
    {
        return;
    }

	fadeOutDuration_ = fadeOutDuration;
	fadeInDuration_ = fadeInDuration;

    onSwitchScene_ = std::move(onSwitchScene);
    track_ = std::move(track);

    timer_ = 0.0f;
    state_ = SceneTransitionState::FadeOut;

    if (track_)
    {
        track_->Begin(TransitionPhase::FadeOut);
        track_->Apply(TransitionPhase::FadeOut, 0.0f);
    }
}

void SceneTransition::Update(float deltaTime)
{
    if (state_ == SceneTransitionState::Idle)
    {
        return;
    }

    timer_ += deltaTime;

    if (!track_)
    {
        Finish();
        return;
    }

    switch (state_)
    {
    case SceneTransitionState::FadeOut:
    {
        float progress = 1.0f;

        if (fadeOutDuration_ > 0.0f)
        {
            progress = timer_ / fadeOutDuration_;
        }

        progress = std::clamp(progress, 0.0f, 1.0f);

        track_->Apply(TransitionPhase::FadeOut, progress);

        if (progress >= 1.0f)
        {
            track_->End(TransitionPhase::FadeOut);

            timer_ = 0.0f;
            state_ = SceneTransitionState::FadeIn;

            if (onSwitchScene_)
            {
                onSwitchScene_();
            }

            track_->Begin(TransitionPhase::FadeIn);
            track_->Apply(TransitionPhase::FadeIn, 0.0f);
        }

        break;
    }

    case SceneTransitionState::FadeIn:
    {
        float progress = 1.0f;

        if (fadeInDuration_ > 0.0f)
        {
            progress = timer_ / fadeInDuration_;
        }

        progress = std::clamp(progress, 0.0f, 1.0f);

        track_->Apply(TransitionPhase::FadeIn, progress);

        if (progress >= 1.0f)
        {
            track_->End(TransitionPhase::FadeIn);
            Finish();
        }

        break;
    }

    case SceneTransitionState::Idle:
        break;
    }
}

bool SceneTransition::IsRunning() const
{
    return state_ != SceneTransitionState::Idle;
}

bool SceneTransition::IsSceneChangeTiming() const
{
	return false;
}

void SceneTransition::BeginFadeOut()
{

}

void SceneTransition::BeginFadeIn()
{

}

void SceneTransition::Finish()
{
    if (onFinish_)
    {
        onFinish_();
    }

    state_ = SceneTransitionState::Idle;
    timer_ = 0.0f;
    onSwitchScene_ = nullptr;
	onFinish_ = nullptr;
    track_.reset();
}

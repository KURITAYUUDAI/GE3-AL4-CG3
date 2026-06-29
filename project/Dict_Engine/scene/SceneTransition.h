#pragma once
#include <functional>
#include <memory>
#include "TransitionTrack.h"

class SceneTransition
{
public:
    enum class SceneTransitionState
    {
        Idle,
        FadeOut,
        FadeIn
    };

    
public:
    void Start(
        float fadeOutDuration,
        float fadeInDuration,
        std::function<void()> onSwitchScene,
        std::unique_ptr<ITransitionTrack> track);

    void Update(float deltaTime);

public:
    const SceneTransitionState& GetState() const { return state_; }
    bool IsRunning() const;
    bool IsSceneChangeTiming() const;

private:
    void BeginFadeOut();
    void BeginFadeIn();
    void Finish();

private:

    SceneTransitionState state_ = SceneTransitionState::Idle;

    float timer_= 0.0f;
    float fadeOutDuration_ = 0.5f;
    float fadeInDuration_ = 0.5f;

    std::function<void()> onSwitchScene_;

    std::unique_ptr<ITransitionTrack> track_;
};


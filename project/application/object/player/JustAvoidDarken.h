#pragma once
#include "PostEffectController.h"
#include "Darken.h"

class JustAvoidDarken
{
public:
    struct Setting
    {
        float duration = 1.0f;
        float maxIntensity = 0.55f;

        // 0.0 ～ 1.0
        // duration全体のうち、暗くなるまでに使う割合
        float attackRate = 0.18f;

        float returnRate = 0.20f;

        Vector4 darkColor = { 0.0f, 0.0f, 0.0f, 1.0f };
    };

private:
    enum class State
    {
        None,
        Attack,
        Keep,
        Return,
        MidReturn
    };

public:
    explicit JustAvoidDarken(PostEffectController* controller)
        : controller_(controller)
    {}

    void SetSetting(const Setting& setting)
    {
        setting_ = setting;
    }

    void Play();

    void Stop();

    void StartReturn(float returnDuration);

    bool IsPlaying() const;

private:
    float ApplyIntensity(float progress) const;

private:
    PostEffectController* controller_ = nullptr;
    PostEffectHandle handle_ = kInvalidPostEffectHandle;

    Setting setting_;

    State state_ = State::None;

    float stateTimer_ = 0.0f;
    float currentIntensity_ = 0.0f;
    float returnStartIntensity_ = 0.0f;
    float returnDuration_ = 0.2f;
};


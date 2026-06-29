#pragma once
#include "TransitionTrack.h"
#include "myMath.h"
#include "PostEffectController.h"
#include "Dissolve.h"

class DissolveTransition : public ITransitionTrack
{
public:
    struct Setting
    {
        float edgeWidth = 0.08f;
        float noiseScale = 45.0f;

        Vector4 fadeColor = { 0, 0, 0, 1 };
        Vector4 edgeColor = { 1, 0.8f, 0.2f, 1 };
    };


public:
    DissolveTransition(PostEffectController* controller)
        : controller_(controller)
    {}

    void SetSetting(const Setting& setting)
    {
        setting_ = setting;
    }

    void Begin(TransitionPhase phase) override;
    void Apply(TransitionPhase phase, float progress) override;
    void End(TransitionPhase phase) override;
    void Cancel() override;

private:
    PostEffectController* controller_ = nullptr;
    PostEffectHandle handle_ = kInvalidPostEffectHandle;
    Setting setting_;
};


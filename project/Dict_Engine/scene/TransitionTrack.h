#pragma once

enum class TransitionPhase
{
    FadeOut,
    FadeIn
};

class ITransitionTrack
{
public:
    virtual ~ITransitionTrack() = default;

    virtual void Begin(TransitionPhase phase) = 0;

    virtual void Apply(TransitionPhase phase, float progress) = 0;

    virtual void End(TransitionPhase phase) = 0;

    virtual void Cancel() = 0;
};



#define NOMINMAX
#include "JustAvoidDarken.h"
#include <algorithm>
#include "time/DeltaTimeManager.h"

void JustAvoidDarken::Play()
{
    if (!controller_)
    {
        return;
    }

    // 連続ジャスト回避時に暗転が多重に積まれないようにする
    if (controller_->IsActive(handle_))
    {
        controller_->Cancel(handle_);
    }

    state_ = State::Attack;
    stateTimer_ = 0.0f;
    currentIntensity_ = 0.0f;

    handle_ = controller_->Emit<Darken>(
        "Darken",
        setting_.duration,
        [this](Darken& darken, float progress, float elapsedTime){
            float intensity = ApplyIntensity(progress);

            stateTimer_ += DeltaTimeManager::GetInstance()->GetDeltaTime(DeltaTimeGroup::UI);
            float keepRate = setting_.duration - (setting_.attackRate + setting_.returnRate);

            switch (state_)
            {
            case State::Attack:
            {
                float t = 1.0f;

                if (setting_.attackRate > 0.0f)
                {
                    t = stateTimer_ / setting_.attackRate;
                }

                t = std::clamp(t, 0.0f, 1.0f);

                currentIntensity_ = setting_.maxIntensity * t;

                if (t >= 1.0f)
                {
                    state_ = State::Keep;
                    stateTimer_ = 0.0f;
                    currentIntensity_ = setting_.maxIntensity;
                }

                break;
            }

            case State::Keep:
            {
                float t = 1.0f;

                if (keepRate > 0.0f)
                {
                    t = stateTimer_ / keepRate;
                }

                t = std::clamp(t, 0.0f, 1.0f);

                currentIntensity_ = setting_.maxIntensity;

                if (t >= 1.0f)
                {
                    state_ = State::Return;
                    stateTimer_ = 0.0f;
                    currentIntensity_ = setting_.maxIntensity;
                }
                break;
            }

            case State::Return:
            {
                float t = 1.0f;

                if (setting_.returnRate > 0.0f)
                {
                    t = stateTimer_ / setting_.returnRate;
                }

                t = std::clamp(t, 0.0f, 1.0f);

                currentIntensity_ = setting_.maxIntensity * (1.0f - t);

                if (t >= 1.0f)
                {
                    currentIntensity_ = 0.0f;
                    darken.SetIntensity(0.0f);

                    controller_->Cancel(handle_);
                    handle_ = kInvalidPostEffectHandle;
                    state_ = State::None;
                    return;
                }

                break;
            }

            case State::MidReturn:
            {
                float t = 1.0f;

                if (returnDuration_ > 0.0f)
                {
                    t = stateTimer_ / returnDuration_;
                }

                t = std::clamp(t, 0.0f, 1.0f);

                currentIntensity_ = returnStartIntensity_ * (1.0f - t);

                if (t >= 1.0f)
                {
                    currentIntensity_ = 0.0f;
                    darken.SetIntensity(0.0f);

                    controller_->Cancel(handle_);
                    handle_ = kInvalidPostEffectHandle;
                    state_ = State::None;
                    return;
                }

                break;
            }

            case State::None:
                currentIntensity_ = 0.0f;
                break;
            }

            darken.SetDarkColor(setting_.darkColor);
            darken.SetIntensity(currentIntensity_);
        });
}

void JustAvoidDarken::Stop()
{
    if (!controller_)
    {
        return;
    }

    if (controller_->IsActive(handle_))
    {
        controller_->Cancel(handle_);
    }

    handle_ = kInvalidPostEffectHandle;
    state_ = State::None;
    currentIntensity_ = 0.0f;
}

void JustAvoidDarken::StartReturn(float returnDuration)
{
    if (!controller_)
    {
        return;
    }

    if (!controller_->IsActive(handle_))
    {
        return;
    }

    state_ = State::MidReturn;
    stateTimer_ = 0.0f;

    returnStartIntensity_ = currentIntensity_;
    returnDuration_ = std::max(returnDuration, 0.0f);
}

bool JustAvoidDarken::IsPlaying() const
{
    if (!controller_)
    {
        return false;
    }

    return controller_->IsActive(handle_);
}

float JustAvoidDarken::ApplyIntensity(float progress) const
{
    progress = std::clamp(progress, 0.0f, 1.0f);

    float attackRate = std::clamp(setting_.attackRate, 0.01f, 0.9f);

    float intensity = 0.0f;

    if (progress < attackRate)
    {
        float t = progress / attackRate;

        // すぐ暗くなる
        intensity = setting_.maxIntensity * t;
    } else
    {
        float t = (progress - attackRate) / (1.0f - attackRate);
        t = std::clamp(t, 0.0f, 1.0f);

        // ゆっくり戻る
        intensity = setting_.maxIntensity * (1.0f - t);
    }

    return std::clamp(intensity, 0.0f, 1.0f);
}

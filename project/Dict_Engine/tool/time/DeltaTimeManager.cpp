#include "DeltaTimeManager.h"
#include <algorithm>
#include <cmath>
#include <cassert>
#include "ImGuiManager.h"

std::unique_ptr<DeltaTimeManager> DeltaTimeManager::instance_ = nullptr;

DeltaTimeManager* DeltaTimeManager::GetInstance()
{
    if (instance_ == nullptr)
    {
        instance_ = std::make_unique<DeltaTimeManager>(ConstructorKey());
    }
    return instance_.get();
}

void DeltaTimeManager::Finalize()
{
    instance_.reset();
}

void DeltaTimeManager::Initialize()
{
    // 全グループをデフォルト値で登録
    for (auto group : {
        DeltaTimeGroup::Player,
        DeltaTimeGroup::Enemy,
        DeltaTimeGroup::Effect,
        DeltaTimeGroup::World,
        DeltaTimeGroup::UI })
    {
        GroupState state;
        state.currentScale = 1.0f;
        state.targetScale = 1.0f;
        state.defaultScale = 1.0f;
        state.phase = GroupState::Phase::Idle;
        groups_[group] = state;
    }
}

void DeltaTimeManager::Update(float rawDeltaTime)
{
    rawDeltaTime_ = rawDeltaTime;
    for (auto& [group, state] : groups_) 
    {
        UpdateGroupState(state, rawDeltaTime);
    }
}

void DeltaTimeManager::ImGuiDebug()
{
#ifdef _DEBUG

    ImGui::Begin("DeltaTime View");

    const char* groupNames[] = { "Player", "Enemy", "Effect", "World", "UI" };
    const char* phaseNames[] = { "Idle", "BlendIn", "Hold", "BlendOut" };

    ImGui::Text("RawDeltaTime : %.4f", rawDeltaTime_);
    ImGui::Text("RawFPS       : %.1f", 1.0f / rawDeltaTime_);
    ImGui::Separator();

    for (auto& [group, state] : groups_)
    {
        int groupIdx = static_cast<int>(group);
        ImGui::Text("[%s]", groupNames[groupIdx]);
        ImGui::SameLine();
        ImGui::Text("Phase: %s", phaseNames[static_cast<int>(state.phase)]);

        // スケールをプログレスバーで可視化（0.0〜2.0の範囲を想定）
        char label[64];
        sprintf_s(label, "%.3f##%d", state.currentScale, groupIdx);
        ImGui::ProgressBar(state.currentScale / 2.0f, ImVec2(-1, 0), label);

        ImGui::Text("  scaled dt : %.4f", rawDeltaTime_ * state.currentScale);
        ImGui::Spacing();
    }


    ImGui::End();

#endif
}

void DeltaTimeManager::UpdateGroupState(GroupState& state, float rawDt) 
{
    switch (state.phase) 
    {
    case GroupState::Phase::BlendIn:
        state.blendTimer += rawDt;
        {
            float t = std::clamp(state.blendTimer / state.blendDuration, 0.0f, 1.0f);
            // ease-out cubic
            float ease = 1.0f - std::pow(1.0f - t, 3.0f);
            state.currentScale = state.defaultScale + (state.targetScale - state.defaultScale) * ease;
            if (t >= 1.0f) 
            {
                state.currentScale = state.targetScale;
                state.holdTimer = 0.0f;
                state.phase = GroupState::Phase::Hold;
            }
        }
        break;

    case GroupState::Phase::Hold:
        state.holdTimer += rawDt;
        if (state.holdTimer >= state.holdTime) 
        {
            state.blendOutTimer = 0.0f;
            state.phase = GroupState::Phase::BlendOut;
        }
        break;

    case GroupState::Phase::BlendOut:
        state.blendOutTimer += rawDt;
        {
            float t = std::clamp(state.blendOutTimer / state.blendOutDuration, 0.0f, 1.0f);
            float ease = std::pow(t, 2.0f); // ease-in quad
            state.currentScale = state.targetScale + (state.defaultScale - state.targetScale) * ease;
            if (t >= 1.0f) 
            {
                state.currentScale = state.defaultScale;
                state.phase = GroupState::Phase::Idle;
            }
        }
        break;

    default: break;
    }
}

void DeltaTimeManager::SetGroupScale(DeltaTimeGroup group, float scale) 
{
    groups_[group].currentScale = scale;
}

void DeltaTimeManager::SetGlobalScale(float scale, bool affectPlayer) 
{
    for (auto& [group, state] : groups_) 
    {
        if (!affectPlayer && group == DeltaTimeGroup::Player) continue;
        state.currentScale = scale;
    }
}

float DeltaTimeManager::GetDeltaTime(DeltaTimeGroup group) const 
{
    auto it = groups_.find(group);
    if (it == groups_.end()) return rawDeltaTime_;
    assert(it != groups_.end() && "未登録のTimeGroup。Initializeを確認してください。");
    return rawDeltaTime_ * it->second.currentScale;
}

void DeltaTimeManager::RequestSlowMotion(DeltaTimeGroup group, float scale,
                                    float blendInTime, float holdTime, float blendOutTime) 
{
    auto& state = groups_[group];
    state.targetScale = scale;
    state.blendDuration = blendInTime;
    state.blendTimer = 0.0f;
    state.holdTimer = 0.0f;
    state.holdTime = holdTime;
    state.blendOutDuration = blendOutTime;
    state.blendOutTimer = 0.0f;
    state.phase = GroupState::Phase::BlendIn;
}

void DeltaTimeManager::RequestOtherSlowMotion(DeltaTimeGroup selfGroup, float scale, float blendInTime, float holdTime, float blendOutTime)
{
    for (auto& [group, state] : groups_)
    {
        if(group == selfGroup) continue;
        if(group == DeltaTimeGroup::UI) continue;

        state.targetScale = scale;
        state.blendDuration = blendInTime;
        state.blendTimer = 0.0f;
        state.holdTimer = 0.0f;
        state.holdTime = holdTime;
        state.blendOutDuration = blendOutTime;
        state.blendOutTimer = 0.0f;
        state.phase = GroupState::Phase::BlendIn;
    }
}

void DeltaTimeManager::ResetAll() 
{
    for (auto& [group, state] : groups_) 
    {
        state.currentScale = 1.0f;
        state.phase = GroupState::Phase::Idle;
    }
}
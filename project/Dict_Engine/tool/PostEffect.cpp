#include "PostEffect.h"
#include "DirectXBase.h"
#include "SrvManager.h"
#include <cassert>

// ---------------------------------------------------------------
//  リソースバリアヘルパー
// ---------------------------------------------------------------
void PostEffect::TransitionResource(
    ID3D12GraphicsCommandList* cmdList,
    ID3D12Resource* resource,
    D3D12_RESOURCE_STATES      before,
    D3D12_RESOURCE_STATES      after)
{
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = resource;
    barrier.Transition.StateBefore = before;
    barrier.Transition.StateAfter = after;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    cmdList->ResourceBarrier(1, &barrier);
}

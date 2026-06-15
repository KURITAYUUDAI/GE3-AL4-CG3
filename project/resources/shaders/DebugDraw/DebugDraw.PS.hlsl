#include "DebugDraw.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

float4 main(PSInput input) : SV_TARGET
{
    return input.color; // 頂点カラーをそのまま出力
}
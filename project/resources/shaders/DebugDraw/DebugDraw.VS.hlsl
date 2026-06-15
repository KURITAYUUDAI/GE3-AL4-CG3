#include "DebugDraw.hlsli"

cbuffer Transform : register(b0)
{
    matrix gViewProjection;
};

struct VSInput
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    // 3D空間の座標を画面座標に変換
    output.position = mul(input.position, gViewProjection);
    output.color = input.color;
    return output;
}
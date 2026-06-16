#include "DebugDraw.hlsli"

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

float4 main(VertexShaderOutput input) : SV_TARGET
{    
    PixelShaderOutput output;

    output.color = input.color;
    
    return output.color; // 頂点カラーをそのまま出力
}
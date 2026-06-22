#include "Fullscreen.hlsli"
#include "../tool/Dissolve.hlsli"

Texture2D<float4> gTexture : register(t0); // 直前のパスのテクスチャ（横ボケ画像）
Texture2D<float4> gMaskTexture : register(t1); // 【追加】最初のメインHDRテクスチャ
SamplerState gSampler : register(s0);

ConstantBuffer<DissolveParams> gDissolve : register(b0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler, input.texcoord);
    
    float mask = gMaskTexture.Sample(gSampler, input.texcoord);
    
    // // エッジの計算
    //float edge = 1.0f - smoothstep(gDissolve.threshold, gDissolve.threshold + 0.03f, mask);
    
    //// 元の色にエッジカラーを加算
    //output.color.rgb += edge * gDissolve.edgeColor.rgb;
    
    ApplyDissolve(mask, gDissolve.threshold, gDissolve.edgeColor, output.color);
    
    return output;
}
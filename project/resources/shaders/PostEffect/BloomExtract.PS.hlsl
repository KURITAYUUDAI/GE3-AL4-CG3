#include "Fullscreen.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// ===== [テキストより独自で変換したポイント] =======
// float32_t4 → float4
//                                         by ChatGPT
// ==================================================

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float4 color = gTexture.Sample(gSampler, input.texcoord);
    
    // 輝度（明るさ）を計算
    float brightness = dot(color.rgb, float3(0.2126f, 0.7152f, 0.0722f));
    
    // 閾値（1.0）を超えた眩しいピクセルだけを出力。それ以外は真っ黒
    if (brightness > 1.2f)
    {
        output.color = color;
    }
    else
    {
        output.color = float4(0.0f, 0.0f, 0.0f, 1.0f);
    }
    
    return output;
}
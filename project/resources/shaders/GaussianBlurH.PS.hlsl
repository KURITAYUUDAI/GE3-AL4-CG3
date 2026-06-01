#include "Fullscreen.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

cbuffer BlurParams : register(b0)
{
    float gSigma;
    int gKernelRadius;
    float2 gTexelSize; // (1/width, 1/height)
}

float Gauss1D(float x, float sigma)
{
    return exp(-(x * x) / (2.0f * sigma * sigma));
}

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    float4 output = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float sum = 0.0f;

    // 線形サンプリング最適化：2ピクセル分の重みを1サンプルにまとめる
    for (int step = -gKernelRadius; step <= gKernelRadius; step += 2)
    {
        float w0 = Gauss1D((float) step, gSigma);
        float w1 = Gauss1D((float) (step + 1), gSigma);
        float wCombined = w0 + w1;

        // バイリニア補間が自動的にw0:w1の比率でブレンドする中間位置
        float offset = (float) step + w1 / wCombined;

        float2 uv = input.texcoord + float2(offset * gTexelSize.x, 0.0f);
        output += gTexture.Sample(gSampler, uv) * wCombined;
        sum += wCombined;
    }

    PixelShaderOutput result;
    result.color = output * rcp(sum);
    return result;
}
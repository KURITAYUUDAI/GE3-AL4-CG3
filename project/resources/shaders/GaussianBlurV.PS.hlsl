#include "Fullscreen.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

cbuffer BlurParams : register(b0)
{
    float gSigma;
    int gKernelRadius;
    float2 gTexelSize;
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

    for (int step = -gKernelRadius; step <= gKernelRadius; step += 2)
    {
        float w0 = Gauss1D((float) step, gSigma);
        float w1 = Gauss1D((float) (step + 1), gSigma);
        float wCombined = w0 + w1;

        float offset = (float) step + w1 / wCombined;

        // 縦方向なのでV成分にオフセットを加算
        float2 uv = input.texcoord + float2(0.0f, offset * gTexelSize.y);
        output += gTexture.Sample(gSampler, uv) * wCombined;
        sum += wCombined;
    }

    PixelShaderOutput result;
    result.color = output * rcp(sum);
    return result;
}
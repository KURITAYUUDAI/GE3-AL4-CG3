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

static const float2 kIndex3x3[3][3] =
{
    { { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f } },
    { { -1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f } },
    { { -1.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } },
};

static const float kPrewittHorizontalKernel[3][3] =
{
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
};

static const float kPrewittVerticalKernel[3][3] =
{
    { -1.0f / 6.0f, -1.0f / 6.0f, -1.0f / 6.0f },
    {  0.0f,  0.0f,  0.0f },
    {  1.0f / 6.0f,  1.0f / 6.0f,  1.0f / 6.0f },
};

float Luminance(float3 color)
{
    return dot(color, float3(0.2126f, 0.7152f, 0.0722f));
}

PixelShaderOutput main(VertexShaderOutput input)
{
    uint width, height; // 1. uvStepSizeの算出
    gTexture.GetDimensions(width, height);
    float2 uvStepSize = float2(rcp(float(width)), rcp(float(height)));
    
    PixelShaderOutput output;
    
    float2 difference = float2(0.0f, 0.0f);
    // 色を輝度に変換して、畳み込みを行っていく。微分Fliter用のKernelになっているので、やること自体は今までの畳み込みと同じ
    for (uint x = 0; x < 3; ++x)
    {
        for (uint y = 0; y < 3; ++y)
        {
            float2 texcoord = input.texcoord + kIndex3x3[x][y] * uvStepSize;
            float3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
            float luminance = Luminance(fetchColor);
            difference.x += luminance * kPrewittHorizontalKernel[x][y];
            difference.y += luminance * kPrewittVerticalKernel[x][y];
        }
    }
    
    // 変化の長さをウェイトとして剛性。ウェイトの決定方法も色々と考えられる。たとえばdifference.xだけ使えば横方向のエッジが検出される
    //float weight = length(difference);
    //weight = saturate(weight);
    
    //output.color.rgb = weight;
    //output.color.a = 1.0f;
    
    float weight = length(difference);
    weight = saturate(weight * 6.0f);
    
    output.color.rgb = (1.0f - weight) * gTexture.Sample(gSampler, input.texcoord).rgb;
    output.color.a = 1.0f;
    
    return output;
}
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
    { { -1.0f,  0.0f }, { 0.0f,  0.0f }, { 1.0f,  0.0f } },
    { { -1.0f,  1.0f }, { 0.0f,  1.0f }, { 1.0f,  1.0f } },
};

static const float kKernel3x3[3][3] =
{
    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f },
    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f },
    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f },
};

static const float2 kIndex5x5[5][5] =
{
    { { -1.0f, -1.0f }, { -0.5f, -1.0f }, { 0.0f, -1.0f }, { 0.5f, -1.0f }, { 1.0f, -1.0f } },
    { { -1.0f, -0.5f }, { -0.5f, -0.5f }, { 0.0f, -0.5f }, { 0.5f, -0.5f }, { 1.0f, -0.5f } },
    { { -1.0f,  0.0f }, { -0.5f,  0.0f }, { 0.0f,  0.0f }, { 0.5f,  0.0f }, { 1.0f,  0.0f } },
    { { -1.0f,  0.5f }, { -0.5f,  0.5f }, { 0.0f,  0.5f }, { 0.5f,  0.5f }, { 1.0f,  0.5f } },
    { { -1.0f,  1.0f }, { -0.5f,  1.0f }, { 0.0f,  1.0f }, { 0.5f,  1.0f }, { 1.0f,  1.0f } },
};

static const float kKernel5x5[5][5] =
{
    { 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f },
    { 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f },
    { 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f },
    { 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f },
    { 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f },
};

PixelShaderOutput main(VertexShaderOutput input)
{
    uint width, height; // 1. uvStepSizeの算出
    gTexture.GetDimensions(width, height);
    float2 uvStepSize = float2(rcp(float(width)), rcp(float(height)));
    
    PixelShaderOutput output;
    output.color.rgb = float3(0.0f, 0.0f, 0.0f);
    output.color.a = 1.0f;
    for (int x = 0; x < 5; ++x)
    {// 2. 5x5ループ
        for (int y = 0; y < 5; ++y)
        {
            // 3. 現在のtexcoordを算出
            float2 texcoord = input.texcoord + kIndex5x5[y][x] * uvStepSize;
            // 4. 色に1/25掛けて足す
            float3 fecthColor = gTexture.Sample(gSampler, texcoord).rgb;
            output.color.rgb += fecthColor * kKernel5x5[y][x];
        }
    }
    
    return output;
}
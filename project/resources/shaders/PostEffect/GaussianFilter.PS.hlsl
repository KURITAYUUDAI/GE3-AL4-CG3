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

static const float PI = 3.14159265f;

float gauss(float x, float y, float sigma)
{
    float exponent = -(x * x + y * y) / (2.0f * sigma * sigma);
    float denominator = 2.0f * PI * sigma * sigma;
    return exp(exponent) / rcp(denominator);
}

float gauss(float x, float sigma)
{
    return 1.0f / (sqrt(2.0f * PI) * sigma) * exp(-(x * x) / (2.0f * sigma * sigma));

}

float4 GaussianBlur(float2 uv, float sigma)
{
    float4 output = 0;
    float sum = 0;
    float weight = 0.0f;
    float4 color = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    uint width, height; // 1. uvStepSizeの算出
    gTexture.GetDimensions(width, height);
    float2 uvStepSize = float2(rcp(float(width)), rcp(float(height)));
    
    for (int kernelStep = -KERNEL_SIZE / 2; kernelStep <= KERNEL_SIZE / 2; ++kernelStep)
    {
        float2 uvOffset = uv + kIndex3x3[1][1] * uvStepSize;
        weight = gauss(kernelStep, sigma);
        output += gTexture.Sample(gSampler, uvOffset) * weight;
        sum += weight;
    }
    output *= (1.0f / sum);
    
    return output;
}
    
PixelShaderOutput main(VertexShaderOutput input)
{
    uint width, height; // 1. uvStepSizeの算出
    gTexture.GetDimensions(width, height);
    float2 uvStepSize = float2(rcp(float(width)), rcp(float(height)));
    
    float weight = 0.0f;
    float kernel3x3[3][3];
    for (int x = 0; x < 3; ++x)
    {
        for (int y = 0; y < 3; ++y)
        {
            kernel3x3[y][x] = gauss(kIndex3x3[y][x].x, kIndex3x3[y][x].y, 2.0f);
            weight += kernel3x3[y][x];
        }
    }
    
    PixelShaderOutput output;
    output.color.rgb = float3(0.0f, 0.0f, 0.0f);
    output.color.a = 1.0f;
    for (int x = 0; x < 3; ++x)
    {// 2. 3x3ループ
        for (int y = 0; y < 3; ++y)
        {
            // 3. 現在のtexcoordを算出
            float2 texcoord = input.texcoord + kIndex3x3[y][x] * uvStepSize;
            // 4. 色に1/25掛けて足す
            float3 fecthColor = gTexture.Sample(gSampler, texcoord).rgb;
            output.color.rgb += fecthColor * kernel3x3[y][x];
        }
    }
    
    output.color.rgb *= rcp(weight); // 正規化
    
    return output;
}
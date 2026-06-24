#include "Fullscreen.hlsli"

Texture2D<float4> gTexture : register(t0); // 直前のパスのテクスチャ（横ボケ画像）
SamplerState gSampler : register(s0);

struct Material
{
    int64_t time;
};
ConstantBuffer<Material> gMaterial : register(b0);

float rand2dTo1d(float2 value, float2 dotDir = float2(12.9898, 78.233))
{
    float2 smallValue = sin(value);
    float random = dot(smallValue, dotDir);
    random = frac(sin(random) * 143758.5453);
    return random;
}

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
	output.color = gTexture.Sample(gSampler, input.texcoord);
    
    float32_t random = rand2dTo1d(input.texcoord * gMaterial.time);
    
    output.color *= float32_t4(random, random, random, 1.0f);
    return output;
}
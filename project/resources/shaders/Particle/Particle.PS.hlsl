#include "Particle.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// ===== [テキストより独自で変換したポイント] =======
// float32_t4 → float4
//                                         by ChatGPT
// ==================================================

struct Material
{
    float4 color;
    
    int enableLighting;
    float shiniess;
    float environmentCoefficient;
    float alphaReference;
    
    row_major float4x4 uvTransform;
};
ConstantBuffer<Material> gMaterial : register(b0);

struct DirectionalLight
{
    float4 color; //!< ライトの色
    float3 direction; //!< ライトの方向
    float intensity; //!< 輝度
};
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), input.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
    float alpha = gMaterial.color.a * textureColor.a;

    if (alpha < gMaterial.alphaReference)
    {
        discard;
    }
    
    PixelShaderOutput output;
    
    output.color = gMaterial.color * textureColor * input.color;
    if(output.color.a <= gMaterial.alphaReference)
    {
        discard;
    }
    
    return output;
}
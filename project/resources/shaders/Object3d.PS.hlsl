#include "object3d.hlsli"

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
    row_major float4x4 uvTransform;
};
ConstantBuffer<Material> gMaterial : register(b0);

struct DirectionalLight
{
    float4 color;     //!< ライトの色
    float3 direction; //!< ライトの方向
    float intensity;  //!< 輝度
};
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    PixelShaderOutput output;
    
    if(gMaterial.enableLighting != 0)   // Lightningする場合
    {
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);   // Normal dot LightDirection
        
        //float cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f); 
        
        output.color.rgb = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        output.color.a = gMaterial.color.a * textureColor.a;
    }
    else
    {
        output.color = gMaterial.color * textureColor;
    }
    
    return output;
}
#include "Object3d.hlsli"

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
    float shiniess;
};
ConstantBuffer<Material> gMaterial : register(b0);

struct DirectionalLight
{
    float4 color;     //!< ライトの色
    float3 direction; //!< ライトの方向
    float intensity;  //!< 輝度
};
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

struct Camera
{
    float3 worldPosition;
};
ConstantBuffer<Camera> gCamera : register(b2);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
    
    PixelShaderOutput output;
    
    if (gMaterial.enableLighting == 0)
    {
        output.color = gMaterial.color * textureColor;
        return output;
    }
    
    if(gMaterial.enableLighting == 1)   // Lightningする場合
    {
        float3 N = normalize(input.normal);
        float3 L = normalize(-gDirectionalLight.direction); // 面→光
        
        //// Lambert
        //float cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
        
        // Half-Lambert
        //float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction); // Normal dot LightDirection
        
        float NdotL = saturate(dot(N, L)); // Normal dot LightDirection
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        
        //output.color.rgb = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        //output.color.a = gMaterial.color.a * textureColor.a;
        
        
        float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
        float3 reflectLight = reflect(-L, N);
        float RdotE = dot(reflectLight, toEye);
        float specularPow = pow(saturate(RdotE), gMaterial.shiniess);
        specularPow *= step(0.0001f, NdotL);
        
        // 拡散反射
        float3 diffuse =
        gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        
        // 鏡面反射
        float3 specular =
        gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float3(1.0f, 1.0f, 1.0f);
        
        // 拡散反射＋鏡面反射
        output.color.rgb = diffuse + specular;
        // アルファは今まで通り
        output.color.a = gMaterial.color.a * textureColor.a;

        return output;
    }
    
    
  
    
    //float3 N = normalize(input.normal);
    
    //output.color = float4(N * 0.5f + 0.5f, 1.0f);

    //return output;
}
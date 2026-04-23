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
struct PointLight
{
    float4 color;    //!< ライトの色
    float3 position; //!< ライトの位置
    float intensity; //!< 輝度
    float radius;    //!< ライトの届く最大距離
    float decay;     //!< 減衰率
    float2 padding;  //!< パディング
};
struct ConstBufferLights
{
    DirectionalLight directionalLight;
    PointLight pointLights[16];
    int numPointLights; // 現在有効なポイントライト数
};
ConstantBuffer<ConstBufferLights> gLights : register(b1);

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
    
    if(gMaterial.enableLighting != 0)   // Lightningする場合
    {
        // 法線ベクトル
        float3 inputNormal = normalize(input.normal);
        // 平行光源の方向ベクトル
        float3 directionalLightDirection = normalize(-gLights.directionalLight.direction);
        // 点光源の入射光ベクトル
        float3 pointLightDirection[16];
        for (int i = 0; i < gLights.numPointLights; i++)
        {
            pointLightDirection[i] = normalize(gLights.pointLights[i].position - input.worldPosition);
        }
        
        
        
        //// Lambert
        //float cos = saturate(dot(normalize(input.normal), -gLights.directionalLight.direction));
        
        // 内積
        float NdotL = saturate(dot(inputNormal, directionalLightDirection)); // Normal dot LightDirection
        // Half Lambert
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
       
        // 視線ベクトル
        float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
        // 反射光ベクトル
        float3 reflectLight = reflect(-directionalLightDirection, inputNormal);
        // 視線と反射光の角度
        float RdotE = dot(reflectLight, toEye);
        // 鏡面反射の強さを計算
        float specularPow = pow(saturate(RdotE), gMaterial.shiniess);
        // 影になる部分の鏡面反射を消す
        specularPow *= step(0.0001f, NdotL);
        
        // 拡散反射
        float3 diffuse = gMaterial.color.rgb * textureColor.rgb * gLights.directionalLight.color.rgb 
            * cos * gLights.directionalLight.intensity;
        
        // 鏡面反射
        float3 specular = gLights.directionalLight.color.rgb * gLights.directionalLight.intensity 
            * specularPow * float3(1.0f, 1.0f, 1.0f);
        
        float3 pointDiffuse[16];
        float3 pointSpecular[16];
        
        for (int j = 0; j < gLights.numPointLights; j++)
        {
            //float distance = length(gLights.pointLights[j].position - input.worldPosition);
            //float factor = 1.0f / (distance * distance);
            
            float distance = length(gLights.pointLights[j].position - input.worldPosition);
            float factor = pow(saturate(-distance / gLights.pointLights[j].radius + 1.0), gLights.pointLights[j].decay);
            
            // 点光源の拡散反射
            pointDiffuse[j] = gMaterial.color.rgb * textureColor.rgb * gLights.pointLights[j].color.rgb 
                * saturate(dot(inputNormal, pointLightDirection[j])) * gLights.pointLights[j].intensity * factor;

            // 点光源の鏡面反射
            float3 pointReflectLight = reflect(-pointLightDirection[j], inputNormal);
            float RdotE_Point = dot(pointReflectLight, toEye);
            float specularPow_Point = pow(saturate(RdotE_Point), gMaterial.shiniess);
            specularPow_Point *= step(0.0001f, saturate(dot(inputNormal, pointLightDirection[j])));
            
            pointSpecular[j] = gLights.pointLights[j].color.rgb * gLights.pointLights[j].intensity * factor
                * specularPow_Point * float3(1.0f, 1.0f, 1.0f);
        }
        
        for (int k = 0; k < gLights.numPointLights; k++)
        {
            diffuse += pointDiffuse[k];
            specular += pointSpecular[k];
        }
        
        // 拡散反射＋鏡面反射
        output.color.rgb = diffuse + specular;
        // アルファは今まで通り
        output.color.a = gMaterial.color.a * textureColor.a;

    }
    else
    {
        output.color = gMaterial.color * textureColor;
    }
    
  
    
    //float3 N = normalize(input.normal);
    
    //output.color = float4(N * 0.5f + 0.5f, 1.0f);

    return output;
}
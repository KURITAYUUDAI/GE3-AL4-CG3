#include "Fullscreen.hlsli"

Texture2D<float4> gTexture : register(t0); // 直前のパスのテクスチャ（横ボケ画像）
SamplerState gSampler : register(s0);

struct Material
{
	float intensity;
	float padding[3];
	float4 darkColor;
};
ConstantBuffer<Material> gMaterial : register(b0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
	output.color = gTexture.Sample(gSampler, input.texcoord);
    
	float3 darkened = lerp(output.color.rgb, gMaterial.darkColor.rgb, gMaterial.intensity);
   
	output.color = float4(darkened, output.color.a);
   
    return output;
}
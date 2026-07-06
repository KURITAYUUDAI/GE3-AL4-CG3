#include "FreetypeFont.hlsli"

Texture2D<float> gTexture : register(t0);
SamplerState gSampler : register(s0);

// ===== [テキストより独自で変換したポイント] =======
// float32_t4 → float4
//                                         by ChatGPT
// ==================================================

struct Material
{
	float4 color;
};
ConstantBuffer<Material> gMaterial : register(b0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
	float4 materialColor = gMaterial.color;
	float4 vertexColor = input.color;
	
	float alpha = gTexture.Sample(gSampler, input.texcoord).r;

    PixelShaderOutput output;
	output.color =
	float4(
		materialColor.rgb * vertexColor.rgb,
		materialColor.a * vertexColor.a * alpha
	);
	
    return output;
}

#include "Fullscreen.hlsli"

Texture2D<float4> gTexture : register(t0); // 直前のパスのテクスチャ（横ボケ画像）
Texture2D<float4> gMainSceneTexture : register(t1); // 【追加】最初のメインHDRテクスチャ
SamplerState gSampler : register(s0);

cbuffer BlurParams : register(b0)
{
    float gSigma;
    int gKernelRadius;
    float2 gTexelSize;
}

float Gauss1D(float x, float sigma)
{
    return exp(-(x * x) / (2.0f * sigma * sigma));
}

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float4 blurOutput = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float sum = 0.0f;
    
    //output.color = gTexture.Sample(gSampler, input.texcoord);
    //return output;

    // --- 元の縦ガウスぼかし処理（提示されたコードのまま） ---
    for (int step = -gKernelRadius; step <= gKernelRadius; step += 2)
    {
        float w0 = Gauss1D((float) step, gSigma);
        float w1 = Gauss1D((float) (step + 1), gSigma);
        float wCombined = w0 + w1;

        float offset = (float) step + w1 / wCombined;
        float2 uv = input.texcoord + float2(0.0f, offset * gTexelSize.y);
        
        blurOutput += gTexture.Sample(gSampler, uv) * wCombined;
        sum += wCombined;
    }
    blurOutput /= sum; // これで上下左右に綺麗にボケた光の画像が完成

    // -----------------------------------------------------------
    // 【ここから追加：最終合成 ＆ トーンマッピング】
    // -----------------------------------------------------------
    // 元のくっきりしたメインHDR画面をサンプリング
    float4 mainColor = gMainSceneTexture.Sample(gSampler, input.texcoord);
    
    // 1. 元の画面に、完成したボケ画像を足し算（1.5fなどで光の強さを調整可能）
    float3 finalColor = mainColor.rgb + blurOutput.rgb * 1.5f;
    
    // 2. トーンマッピング（Reinhard法）
    // 1.0を超えて限界突破している輝度を、モニターが表示できる0.0〜1.0に滑らかに圧縮
    //finalColor = finalColor / (finalColor + float3(1.0f, 1.0f, 1.0f));
    
    output.color = float4(finalColor, mainColor.a);
    return output;
}
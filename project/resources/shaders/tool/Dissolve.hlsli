struct DissolveParams
{
    float threshold;
    float3 padding;
    float4 edgeColor;
};

void ApplyDissolve(float maskValue, float threshold, float4 edgeColor, inout float4 outColor)
{
    // threshold以下ならピクセルを破棄
    if (maskValue <= threshold)
    {
        discard;
    }
    
    // エッジの計算
    float edge = 1.0f - smoothstep(threshold, threshold + 0.03f, maskValue);
    
    // 元の色にエッジカラーを加算
    outColor.rgb += edge * edgeColor.rgb;
    
    return;
}
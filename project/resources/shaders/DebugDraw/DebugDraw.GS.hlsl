#include "DebugDraw.hlsli"

[maxvertexcount(2)]
void main(line VertexShaderOutput input[2], inout LineStream<VertexShaderOutput> output)
{
    
    // 両端点が両方カメラ背後（w <= 0）なら、線分全体を破棄
    if (input[0].position.w <= 0.0f || input[1].position.w <= 0.0f)
    {
        return;
    }
    
    output.Append(input[0]);
    output.Append(input[1]);
    
    //VertexShaderOutput v0 = input[0];
    //VertexShaderOutput v1 = input[1];

    //// wが負なら赤、正なら緑に強制的に色を変える
    //v0.color = (v0.position.w <= 0.0f) ? float4(1, 0, 0, 1) : float4(0, 1, 0, 1);
    //v1.color = (v1.position.w <= 0.0f) ? float4(1, 0, 0, 1) : float4(0, 1, 0, 1);
    
    //output.Append(v0);
    //output.Append(v1);
}
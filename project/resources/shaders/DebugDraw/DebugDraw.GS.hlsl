#include "DebugDraw.hlsli"

float PlaneDistance(float4 p, int plane)
{
    // DirectX clip space:
    // -w <= x <= w
    // -w <= y <= w
    //  0 <= z <= w

    if (plane == 0)
        return p.x + p.w; // Left
    if (plane == 1)
        return p.w - p.x; // Right
    if (plane == 2)
        return p.y + p.w; // Bottom
    if (plane == 3)
        return p.w - p.y; // Top
    if (plane == 4)
        return p.z; // Near
    if (plane == 5)
        return p.w - p.z; // Far

    return 1.0f;
}

VertexShaderOutput LerpVertex(VertexShaderOutput a, VertexShaderOutput b, float t)
{
    VertexShaderOutput o;
    o.position = lerp(a.position, b.position, t);
    o.color = lerp(a.color, b.color, t);
    return o;
}

bool ClipLineOnePlane(inout VertexShaderOutput a, inout VertexShaderOutput b, int plane)
{
    float da = PlaneDistance(a.position, plane);
    float db = PlaneDistance(b.position, plane);

    bool insideA = da >= 0.0f;
    bool insideB = db >= 0.0f;

    // 両方外側
    if (!insideA && !insideB)
    {
        return false;
    }

    // 両方内側
    if (insideA && insideB)
    {
        return true;
    }

    // 交点
    float t = da / (da - db);
    VertexShaderOutput hit = LerpVertex(a, b, t);

    if (!insideA)
    {
        a = hit;
    }
    else
    {
        b = hit;
    }

    return true;
}

bool ClipLineFrustum(inout VertexShaderOutput a, inout VertexShaderOutput b)
{
    [unroll]
    for (int i = 0; i < 6; ++i)
    {
        if (!ClipLineOnePlane(a, b, i))
        {
            return false;
        }
    }

    return true;
}

[maxvertexcount(2)]
void main(line VertexShaderOutput input[2], inout LineStream<VertexShaderOutput> output)
{
    VertexShaderOutput a = input[0];
    VertexShaderOutput b = input[1];

    if (!ClipLineFrustum(a, b))
    {
        return;
    }

    output.Append(a);
    output.Append(b);
}

//[maxvertexcount(2)]
//void main(line VertexShaderOutput input[2], inout LineStream<VertexShaderOutput> output)
//{
    
//    // 両端点が両方カメラ背後（w <= 0）なら、線分全体を破棄
//    if (input[0].position.w <= 0.0f || input[1].position.w <= 0.0f)
//    {
//        return;
//    }
    
//    output.Append(input[0]);
//    output.Append(input[1]);
    
//    //VertexShaderOutput v0 = input[0];
//    //VertexShaderOutput v1 = input[1];

//    //// wが負なら赤、正なら緑に強制的に色を変える
//    //v0.color = (v0.position.w <= 0.0f) ? float4(1, 0, 0, 1) : float4(0, 1, 0, 1);
//    //v1.color = (v1.position.w <= 0.0f) ? float4(1, 0, 0, 1) : float4(0, 1, 0, 1);
    
//    //output.Append(v0);
//    //output.Append(v1);
//}
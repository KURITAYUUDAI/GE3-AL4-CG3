
struct Vertex
{
	float32_t4 position;
	float32_t3 texcoord;
	float32_t3 normal;
};

struct VertexInfluence
{
	float32_t4 weight;
	int32_t4 index;
};

struct SkinningInformation
{
	uint32_t numVertices;
};

struct Well
{
	row_major float4x4 skeletonSpaceMatrix;
	row_major float4x4 skeletonSpaceInverseTransposeMatrix;
};

struct Skinned
{
	float4 position;
	float3 normal;
};

Skinned Skinning(Vertex input, Well well, VertexInfluence influence)
{
	Skinned skinned;
    
    // 位置の変換
	skinned.position = mul(input.position, well.skeletonSpaceMatrix) * influence.weight.x;
	skinned.position += mul(input.position, well.skeletonSpaceMatrix) * influence.weight.y;
	skinned.position += mul(input.position, well.skeletonSpaceMatrix) * influence.weight.z;
	skinned.position += mul(input.position, well.skeletonSpaceMatrix) * influence.weight.w;
	skinned.position.w = 1.0f; // 確実に1を入れる
    
    // 法線の変換
	skinned.normal = mul(input.normal, (float3x3) well.skeletonSpaceInverseTransposeMatrix) * influence.weight.x;
	skinned.normal += mul(input.normal, (float3x3) well.skeletonSpaceInverseTransposeMatrix) * influence.weight.y;
	skinned.normal += mul(input.normal, (float3x3) well.skeletonSpaceInverseTransposeMatrix) * influence.weight.z;
	skinned.normal += mul(input.normal, (float3x3) well.skeletonSpaceInverseTransposeMatrix) * influence.weight.w;
	skinned.normal = normalize(skinned.normal); // 正規化して戻してあげる
    
	return skinned;
}

// SkinningObject3d.VS.hlslで作ったものと同じParticle
StructuredBuffer<Well> gMatrixPalette : register(t0);
// VertexBufferViewのstream0として利用していた入力頂点
StructuredBuffer<Vertex> gInputVertices : register(t1);
// VertexBufferViewのstream1として利用していた入力インフルエンス
StructuredBuffer<VertexInfluence> gInfluences : register(t2);
// Skinning計算後の頂点データ。SkinnedVertex
RWStructuredBuffer<Vertex> gOutputVertices : register(u0);
// Skinningに関するちょっとした情報
ConstantBuffer<SkinningInformation> gSkinningInformation : register(b0);


[numthreads(1024, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID)
{	
	uint32_t vertexIndex = DTid.x;
	if(vertexIndex < gSkinningInformation.numVertices)
	{
		// 必要なデータをStructuredBufferから取得
		Vertex input = gInputVertices[vertexIndex];
		VertexInfluence influence = gInfluences[vertexIndex];
		
		// skinning後の頂点を計算
		Vertex skinned;
		skinned.texcoord = input.texcoord;
		
		skinned = Skinning(input, gMatrixPalette[influence.index.x], influence);
		
		// 計算結果をRWStructuredBufferに書き込む
		gOutputVertices[vertexIndex] = skinned;
	}
	else
	{
		return;
	}
}
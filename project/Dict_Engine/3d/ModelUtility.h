#pragma once
#include "DirectXBase.h"
#include "myMath.h"

struct VertexData
{
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
	Vector4 color;
};


struct MaterialData
{
	std::string textureFilePath;
	uint32_t textureIndex = 0;
	Vector4 color;
};

struct Material
{
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
	float shininess;
	float environmentCoefficient;
};

struct Mesh
{
	std::vector<VertexData> vertices;	//!< 頂点データ
	MaterialData material;

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	// バッファリソース内のデータを指すポインタ
	VertexData* vertexData_ = nullptr;
	// バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	// マテリアル用バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;
	// バッファリソース内のデータを指すポインタ
	Material* materialData_ = nullptr;
};

struct ModelData
{
	std::vector<Mesh> meshes;
};
#pragma once
#include "DirectXBase.h"
#include "myMath.h"
#include "Quaternion.h"

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
	Vector4 color = {1.0f, 1.0f, 1.0f, 1.0f};
	float alphaReference = 0.5f;
};

struct Material
{
	Vector4 color;

	int32_t enableLighting;
	float shininess;
	float environmentCoefficient;
	float alphaReference;

	Matrix4x4 uvTransform;
};

struct Node
{
	QuaternionTransform transform;
	Matrix4x4 localMatrix;
	std::string name;

	std::vector<Node> children;
};

struct Mesh
{
	std::vector<VertexData> vertices;	//!< 頂点データ
	std::vector<uint32_t> indices;	//!< インデックスデータ
	MaterialData material;
	Node rootNode;

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	// バッファリソース内のデータを指すポインタ
	VertexData* vertexData_ = nullptr;
	// バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	// インデックスリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_ = nullptr;
	// バッファリソース内のデータを指すポインタ
	uint32_t* indexData_ = nullptr;
	// バッファリソースの使い道を補足するバッファビュー
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	// マテリアル用バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;
	// バッファリソース内のデータを指すポインタ
	Material* materialData_ = nullptr;
};

struct ModelData
{
	std::vector<Mesh> meshes;
};
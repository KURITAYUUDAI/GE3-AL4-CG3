#pragma once
#include "DirectXBase.h"
#include "myMath.h"

class ModelBase;

// 3Dモデル
class Model
{
public:

	struct VertexData
	{
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};


	struct MaterialData
	{
		std::string textureFilePath;
		uint32_t textureIndex = 0;
		Vector4 color;
	};


	struct ModelData
	{
		std::vector<VertexData> vertices;	//!< 頂点データ
		MaterialData material;
	};

	struct Material
	{
		Vector4 color;
		int32_t enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
		float shininess;
	};


public:

	void Initialize(ModelBase* modelBase, const std::string& directoryPath, const std::string& filename);

	void Update();

	void Draw(const UINT& instanceCount);

	void Finalize();

	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename, const std::string& mtlname);

	void LoadObjFile(const std::string& directoryPath, const std::string& filename);

	void SetTexture(const std::string& directoryFilePath);

	void ResetTexture();

private:

	// VertexResourceを作成
	void CreateVertexResource();

	// MaterialResourceを作成
	void CreateMaterialResource();

	// 球作成
	void CreateSphere();

public:	// 外部入出力

	// セッター
	void SetInstanceCount(const UINT& instanceCount) { instanceCount_ = instanceCount; }
	void SetEnableLighting(const int32_t& enableLighting) { materialData_->enableLighting = enableLighting; }

	// ゲッター
	const UINT& GetInstanceCount() const { return instanceCount_; }
	const int32_t GetEnableLighting() const { return materialData_->enableLighting; }

private:

	ModelBase* modelBase_;

	// Objファイルのデータ
	ModelData modelData_;

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

	bool isSphere_ = false;

	UINT instanceCount_ = 1;

	std::string materialTextureFilePath_;
	uint32_t materialTextureIndex_ = 0;
};


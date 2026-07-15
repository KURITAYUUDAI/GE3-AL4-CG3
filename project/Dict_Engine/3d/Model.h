#pragma once
#include "DirectXBase.h"
#include "myMath.h"
#include "ModelManager.h"
#include "ModelUtility.h"

#include <assimp/scene.h>

// 3Dモデル
class Model
{
public:

	

	


public:

	void Initialize();

	void CreateResources();

	void Update();

	void Draw(const UINT& instanceCount);

	void Finalize();

	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename, const std::string& mtlname);

	// 球作成
	void CreateSphere();

	// 箱作成
	void CreateBox();

	void LoadFromFile(const std::string& directoryPath, const std::string& filename);

	void InsertMesh(Mesh mesh);

	void SetTexture(const std::string& directoryFilePath, uint32_t meshIndex);

	void ResetTexture(uint32_t meshIndex);

private:

	Node ReadNode(aiNode* node);

	// VertexResourceを作成
	void CreateVertexResource();

	// MaterialResourceを作成
	void CreateMaterialResource();

	
public:	// 外部入出力

	// セッター
	void SetModelData(const ModelData& modelData) { modelData_ = modelData; }
	void SetInstanceCount(const UINT& instanceCount) { instanceCount_ = instanceCount; }
	
	void SetColor(const Vector4& color, uint32_t meshIndex){ modelData_.meshes[meshIndex].materialData_->color = color; }
	void SetEnableLighting(const int32_t& enableLighting, uint32_t meshIndex) { modelData_.meshes[meshIndex].materialData_->enableLighting = enableLighting; }
	void SetUVTransform(const Transform& uvTransform, uint32_t meshIndex);
	void SetEnvironmentCoefficient(const float& environmentCoefficient, uint32_t meshIndex) { modelData_.meshes[meshIndex].materialData_->environmentCoefficient = environmentCoefficient; }
	void SetAlphaReference(const float alphaReference, uint32_t meshIndex){modelData_.meshes[meshIndex].materialData_->alphaReference = alphaReference; }
	
	

	// ゲッター
	const UINT& GetInstanceCount() const { return instanceCount_; }
	
	const Vector4& GetColor(uint32_t meshIndex) const {return modelData_.meshes[meshIndex].materialData_->color; }
	const int32_t& GetEnableLighting(uint32_t meshIndex) const { return modelData_.meshes[meshIndex].materialData_->enableLighting; }
	const Matrix4x4& GetUVTransform(uint32_t meshIndex) const { return modelData_.meshes[meshIndex].materialData_->uvTransform; }
	const float& GetShininess(uint32_t meshIndex) const { return modelData_.meshes[meshIndex].materialData_->shininess; }
	const float& GetEnvironmentCoefficient(uint32_t meshIndex) const { return modelData_.meshes[meshIndex].materialData_->environmentCoefficient; }
	const float& GetAlphaReference(uint32_t meshIndex) const { return modelData_.meshes[meshIndex].materialData_->alphaReference; }

	Mesh& GetMesh(uint32_t meshIndex) { return modelData_.meshes[meshIndex]; }
	const Node& GetRootNode(uint32_t meshIndex) { return modelData_.meshes[meshIndex].rootNode; }

private:

	ModelManager* modelManager_ = nullptr;

	// モデルのデータ
	ModelData modelData_;

	//// バッファリソース
	//Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	//// バッファリソース内のデータを指すポインタ
	//VertexData* vertexData_ = nullptr;
	//// バッファリソースの使い道を補足するバッファビュー
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	//// マテリアル用バッファリソース
	//Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;
	//// バッファリソース内のデータを指すポインタ
	//Material* materialData_ = nullptr;

	bool isSphere_ = false;

	UINT instanceCount_ = 1;

	std::string materialTextureFilePath_;
	uint32_t materialTextureIndex_ = 0;
};


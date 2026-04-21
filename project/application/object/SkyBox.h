#pragma once
#include "DirectXBase.h"
#include "object3d.h"
#include "myMath.h"
#include "Model.h"

class SkyBox
{
public:
	struct TransformationMatrix
	{
		Matrix4x4 WVP;
		Matrix4x4 World;
	};

public:
	void Initialize();
	void Update();
	void Draw();
	void Finalize();

public:
	void SetCamera(Camera* camera){ camera_ = camera; }
	
private:

	// IndexResourceを作成
	void CreateIndexResource();

	// TransformationMatrixResourceを作成
	void CreateTransformationMatrixResource();

	// VertexResourceを作成
	void CreateVertexResource();

	// MaterialResourceを作成
	void CreateMaterialResource();

	Model::ModelData CreateSkyBox();

private:
	std::string psoName_ = "SkyBox";
	PSOManager::BlendMode blendMode_ = PSOManager::BlendMode::Normal;
	PSOManager::FillMode fillMode_ = PSOManager::FillMode::kSolid;

	Camera* camera_;
	Model::ModelData modelData_;

	Transform transform_;

	// IndexBufferResource
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_ = nullptr;
	// IndexBufferResourceにデータを書き込むためのアドレスを指すポインタ
	uint32_t* indexData_ = nullptr;
	// IndexBufferView
	D3D12_INDEX_BUFFER_VIEW indexBufferView;


	// 座標変換行列用バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_ = nullptr;
	// バッファリソース内のデータを指すポインタ
	TransformationMatrix* transformationMatrixData_ = nullptr;

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	// バッファリソース内のデータを指すポインタ
	Model::VertexData* vertexData_ = nullptr;
	// バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	// マテリアル用バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;
	// バッファリソース内のデータを指すポインタ
	Model::Material* materialData_ = nullptr;

};
#pragma once
#include "myMath.h"
#include "DirectXBase.h"

class SpriteBase;

class Sprite
{
public:

	struct VertexData
	{
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};

	struct Material
	{
		Vector4 color;
		int32_t enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
	};

	struct TransformationMatrix
	{
		Matrix4x4 WVP;
		Matrix4x4 World;
	};

public:

	void Initialize(SpriteBase* spriteBase, const D3D12_GPU_DESCRIPTOR_HANDLE& textureSrvHandle);

	void Update();

	void Draw();

	void Finalize();

public:		// 外部入出力

	void SetScale(const Vector3& scale){ transform_.scale = scale; }
	void SetRotate(const Vector3& rotate){ transform_.rotate = rotate; }
	void SetTranslate(const Vector3& translate){ transform_.translate = translate; }

	void SetUVScale(const Vector3& scale){ uvTransform_.scale = scale; }
	void SetUVRotate(const Vector3& rotate){ uvTransform_.rotate = rotate; }
	void SetUVTranslate(const Vector3& translate){ uvTransform_.translate = translate; }

	void SetTextureSRVHandle(const D3D12_GPU_DESCRIPTOR_HANDLE& textureSrvHandle)
	{ textureSrvHandle_ = textureSrvHandle; }

private:	// 静的関数

	// VertexResourceをIndex方式で作成
	void CreateVertexResource();

	// MaterialResourceを作成
	void CreateMaterialResource();

	// TransformationMatrixResourceを作成
	void CreateTransformationMatrixResource();

private:	// 静的変数

	SpriteBase* spriteBase_ = nullptr;

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_ = nullptr;

	// バッファリソース内のデータを指すポインタ
	VertexData* vertexData_ = nullptr;
	uint32_t* indexData_ = nullptr;

	// バッファリソースの使い道を補足するバッファビュー
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};


	// マテリアル用バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;

	// バッファリソース内のデータを指すポインタ
	Material* materialData_ = nullptr;

	// 座標変換行列用バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_ = nullptr;

	// データを書き込む
	TransformationMatrix* transformationMatrixData_ = nullptr;

	// トランスフォーム
	Transform transform_ = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
	// ワールド行列
	Matrix4x4 worldMatrix_ = {};
	// WVP行列
	Matrix4x4 worldViewProjectionMatrix_ = {};


	// UVトランスフォーム
	Transform uvTransform_ = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
	// UVトランスフォーム行列
	Matrix4x4 uvTransformMatrix_ = {};

	// テクスチャ―ハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandle_;
};
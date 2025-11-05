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

	void Initialize(SpriteBase* spriteBase, std::string textureFilePath);

	void Update();

	void Draw();

	void Finalize();

public:		// 外部入出力

	// セッター
	void SetAnchorPoint(const Vector2& anchorPoint){ anchorPoint_ = anchorPoint; }

	void SetPosition(const Vector2& position){ position_ = position; }
	void SetRotation(const float& rotation){ rotation_ = rotation; }
	void SetSize(const Vector2& size){ size_ = size; }
	void SetColor(const Vector4& color){ materialData_->color = color; }

	void SetTextureLeftTop(const Vector2& textureLeftTop){ textureLeftTop_ = textureLeftTop; }
	void SetTextureSize(const Vector2& textureSize){ textureSize_ = textureSize; }
	void SetFlipX(const bool& isFlipX){ isFlipX_ = isFlipX; }
	void SetFlipY(const bool& isFlipY){ isFlipY_ = isFlipY; }

	void SetUVScale(const Vector3& scale){ uvTransform_.scale = scale; }
	void SetUVRotate(const Vector3& rotate){ uvTransform_.rotate = rotate; }
	void SetUVTranslate(const Vector3& translate){ uvTransform_.translate = translate; }

	void SetTexture(std::string textureFilePath);

	// ゲッター
	const Vector2& GetAnchorPoint(){ return anchorPoint_; }

	const Vector2 GetPosition(){ return position_; }
	const float GetRotation(){ return rotation_; }
	const Vector2 GetSize(){ return size_; }
	const Vector4& GetColor() const { return materialData_->color; }

	const Vector2& GetTextureLeftTop(){ return textureLeftTop_; }
	const Vector2& GetTextureSize(){ return textureSize_; }
	const bool& GetIsFlipX(){ return isFlipX_; }
	const bool& GetIsFlipY(){ return isFlipY_; }

	Transform GetUVTransform() { return uvTransform_; }

	


private:	// 静的関数

	// VertexResourceをIndex方式で作成
	void CreateVertexResource();

	// MaterialResourceを作成
	void CreateMaterialResource();

	// TransformationMatrixResourceを作成
	void CreateTransformationMatrixResource();

	// テクスチャサイズをイメージに合わせる
	void AdjustTextureSize();

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

	// アンカーポイント
	Vector2 anchorPoint_ = {0.0f, 0.0f};

	// 位置
	Vector2 position_ = { 0.0f, 0.0f };
	// 回転
	float rotation_ = 0.0f;
	// サイズ
	Vector2 size_ = { 100.0f, 100.0f };

	// トランスフォーム
	Transform transform_ = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
	// ワールド行列
	Matrix4x4 worldMatrix_ = {};
	// WVP行列
	Matrix4x4 worldViewProjectionMatrix_ = {};

	// テクスチャ左上座標
	Vector2 textureLeftTop_ = { 0.0f, 0.0f };
	// テクスチャ切りだしサイズ
	Vector2 textureSize_ = { 0.0f, 0.0f };

	// 左右フリップ
	bool isFlipX_ = false;
	// 上下フリップ
	bool isFlipY_ = false;

	// UVトランスフォーム
	Transform uvTransform_ = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
	// UVトランスフォーム行列
	Matrix4x4 uvTransformMatrix_ = {};

	

	// テクスチャ番号
	uint32_t textureIndex_ = 0;
};
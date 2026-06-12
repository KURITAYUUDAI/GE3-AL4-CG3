#pragma once
#include "DirectXBase.h"
#include "myMath.h"

class WorldTransform
{
public:

	struct TransformationMatrix
	{
		Matrix4x4 WVP;
		Matrix4x4 World;
	};

public:

	void Initialize();

	void UpdateMatrix(const Matrix4x4* worldMatrix = nullptr);
	void TransferMatrix(const Matrix4x4& viewProjection);

	void Finalize();

	void SetCBufferTransformationResource(UINT rootParamaterIndex);

	static void AdvanceFrame() { ++currentFrame_; }

private:

	void CreateTransformationMatrixResource();

public:

	Vector3 scale_ = {1.0f, 1.0f, 1.0f};
	Vector3 rotate_ = {0.0f, 0.0f, 0.0f};
	Vector3 translate_{ 0.0f, 0.0f, 0.0f };

	Matrix4x4 worldMatrix_;

	WorldTransform* parent_ = nullptr;

	static uint32_t currentFrame_;
	

private:

	// 座標変換行列用バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_ = nullptr;
	// バッファリソース内のデータを指すポインタ
	TransformationMatrix* transformationMatrixData_;

	uint32_t lastUpdateFrame_ = 0;
};


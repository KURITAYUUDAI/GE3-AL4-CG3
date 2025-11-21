#pragma once
#include "myMath.h"
#include "DirectXBase.h"

class Model;

class Camera;

class ParticleBase;

class Particle
{

public:

	struct TransformationMatrix
	{
		Matrix4x4 WVP;
		Matrix4x4 World;
	};

	struct DirectionalLight
	{
		Vector4 color;		//!< ライトの色
		Vector3 direction;	//!< ライトの方向
		float intensity;	//!< 輝度
	};

public:	// メンバ関数

	void Initialize(ParticleBase* particleBase);

	void Update();

	void Draw();

	void Finalize();

public:	// 外部入出力

	// セッター
	void SetModel(const std::string& filePath);
	void SetCamera(Camera* camera){ camera_ = camera; }

	void SetScale(const Vector3& scale){ transform_.scale = scale; }
	void SetRotate(const Vector3& rotate){ transform_.rotate = rotate; }
	void SetTranslate(const Vector3& translate){ transform_.translate = translate; }

	void SetModelInstanceCount(const UINT instanceCount);

	// ゲッター
	const Vector3& GetScale() const { return transform_.scale; }
	const Vector3& GetRotate() const { return transform_.rotate; }
	const Vector3& GetTranslate() const { return transform_.translate; }

	const UINT& GetModelInstanceCount() const;

private: // 静的関数

	// TransformationMatrixResourceを作成
	void CreateTransformationMatrixResource();

	//// DirectionalLightResourceを作成
	//void CreateDirectionalLightResource();

	// InstancinResourceを作成（Particle用）
	void CreateInstancingResource();

private:

	// パーティクル共通処理
	ParticleBase* particleBase_ = nullptr;

	// モデル
	Model* model_ = nullptr;

	// カメラ
	Camera* camera_ = nullptr;

	// 座標変換行列用バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_ = nullptr;
	// バッファリソース内のデータを指すポインタ
	TransformationMatrix* transformationMatrixData_ = nullptr;

	//// 平行光源用のリソースを作成
	//Microsoft::WRL::ComPtr<ID3D12Resource> DirectionalLightResource_ = nullptr;
	//// バッファリソース内のデータを指すポインタ
	//DirectionalLight* directionalLightData = nullptr;

	// トランスフォーム
	Transform transform_;

	// インスタンス数（Particle用）
	uint32_t kNumInstance_;

	// インスタンスデータ用バッファリソース（Particle用）
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource_ = nullptr;
	// インスタンスデータ用バッファリソース内のデータを指すポインタ（Particle用）
	TransformationMatrix* instancingDara_ = nullptr;
};


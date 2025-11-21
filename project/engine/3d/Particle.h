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

	void Initialize(ParticleBase* particleBase, const uint32_t& instanceNum);

	void Update();

	void Draw();

	void Finalize();

public:	// 外部入出力

	// セッター
	void SetModel(const std::string& filePath);
	void SetCamera(Camera* camera){ camera_ = camera; }

	//void SetScale(const Vector3& scale){ transform_.scale = scale; }
	//void SetRotate(const Vector3& rotate){ transform_.rotate = rotate; }
	//void SetTranslate(const Vector3& translate){ transform_.translate = translate; }

	// ゲッター
	const UINT& GetInstanceCount() const { return instanceNum_; }

	//const Vector3& GetScale() const { return transform_.scale; }
	//const Vector3& GetRotate() const { return transform_.rotate; }
	//const Vector3& GetTranslate() const { return transform_.translate; }



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

	// インスタンス数
	uint32_t instanceNum_;

	// インスタンスデータ用バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource_ = nullptr;
	// インスタンスデータ用バッファリソース内のデータを指すポインタ
	TransformationMatrix* instancingData_ = nullptr;
	// インスタンス用SRVのインデックス
	uint32_t instancingSrvIndex_;
	// インスタンス用SRVのCPUハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU_;
	// インスタンス用SRVのGPUハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU_;

	// 座標変換行列用バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_ = nullptr;
	// バッファリソース内のデータを指すポインタ
	TransformationMatrix* transformationMatrixData_ = nullptr;

	//// 平行光源用のリソースを作成
	//Microsoft::WRL::ComPtr<ID3D12Resource> DirectionalLightResource_ = nullptr;
	//// バッファリソース内のデータを指すポインタ
	//DirectionalLight* directionalLightData = nullptr;

	// トランスフォーム
	std::vector<Transform> transforms_;

};


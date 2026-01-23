#pragma once
#include "myMath.h"
#include "DirectXBase.h"

class Model;

class Camera;

class Object3dBase;

class Object3d
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

	struct CameraForGPU
	{
		Vector3 worldPosition;
		float pad;
	};

public:	// メンバ関数
	
	void Initialize(Object3dBase* object3dBase);

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

	void SetEnableLighting(const int32_t& enableLighting);

	void SetLightColor(const Vector4& color){ directionalLightData->color = color; }
	void SetLightDirection(const Vector3& direction){ directionalLightData->direction = direction; }
	void SetLightIntensity(const float& intensity){ directionalLightData->intensity = intensity; }

	// ゲッター
	const Vector3& GetScale() const { return transform_.scale; }
	const Vector3& GetRotate() const { return transform_.rotate; }
	const Vector3& GetTranslate() const { return transform_.translate; }

	const int32_t& GetEnableLighting() const { return enableLighting_; }

	const Vector4& GetLightColor() const { return directionalLightData->color; }
	const Vector3& GetLightDirection() const { return directionalLightData->direction; }
	const float& GetLightIntensity() const { return directionalLightData->intensity; }


private: // 静的関数

	// TransformationMatrixResourceを作成
	void CreateTransformationMatrixResource();

	// DirectionalLightResourceを作成
	void CreateDirectionalLightResource();

	// カメラ用リソースを作成
	void CreateCameraResource();

private:

	// 3Dオブジェクト共通処理
	Object3dBase* object3dBase_ = nullptr;

	// モデル
	Model* model_ = nullptr;

	// カメラ
	Camera* camera_ = nullptr;

	// 座標変換行列用バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_ = nullptr;
	// バッファリソース内のデータを指すポインタ
	TransformationMatrix* transformationMatrixData_ = nullptr;

	// 平行光源用のリソースを作成
	Microsoft::WRL::ComPtr<ID3D12Resource> DirectionalLightResource_ = nullptr;
	// バッファリソース内のデータを指すポインタ
	DirectionalLight* directionalLightData = nullptr;

	// カメラ用リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_ = nullptr;
	// バッファリソース内のデータを指すポインタ
	CameraForGPU* cameraData_ = nullptr;
	
	// トランスフォーム
	Transform transform_;

	// ライティング有効無効
	int32_t enableLighting_ = true;
};


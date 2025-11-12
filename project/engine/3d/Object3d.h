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

	// ゲッター
	const Vector3& GetScale() const { return transform_.scale; }
	const Vector3& GetRotate() const { return transform_.rotate; }
	const Vector3& GetTranslate() const { return transform_.translate; }

private: // 静的関数

	// TransformationMatrixResourceを作成
	void CreateTransformationMatrixResource();

	// DirectionalLightResourceを作成
	void CreateDirectionalLightResource();

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
	
	// トランスフォーム
	Transform transform_;

};


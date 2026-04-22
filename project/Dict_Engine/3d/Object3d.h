#pragma once
#include "myMath.h"
#include "DirectXBase.h"
#include "Object3dManager.h"
#include "PSOManager.h"

class Model;

class Camera;

class Object3d
{

public:

	struct TransformationMatrix
	{
		Matrix4x4 WVP;
		Matrix4x4 World;
	};

public:	// メンバ関数
	
	void Initialize();

	void Update();

	void DrawingCommon();

	void Draw();

	void Finalize();

public:	// 外部入出力

	// セッター
	void SetPsoName(const std::string& psoName){ psoName_ = psoName; }
	void SetBlendMode(const PSOManager::BlendMode& blendMode){ blendMode_ = blendMode; }
	void SetFillMode(const PSOManager::FillMode& fillMode){ fillMode_ = fillMode; }

	void SetModel(const std::string& filePath);
	void SetModel(Model* model){ model_ = model; }
	void SetCamera(Camera* camera){ camera_ = camera; }

	void SetScale(const Vector3& scale){ transform_.scale = scale; }
	void SetRotate(const Vector3& rotate){ transform_.rotate = rotate; }
	void SetTranslate(const Vector3& translate){ transform_.translate = translate; }
	void SetTransform(const Transform& transform){ transform_ = transform; }

	void SetEnableLighting(const int32_t& enableLighting);

	// ゲッター
	const std::string& GetPsoName() const { return psoName_; }
	const PSOManager::BlendMode& GetBlendMode() const { return blendMode_; }
	const PSOManager::FillMode& GetFillMode() const { return fillMode_; }

	const Vector3& GetScale() const { return transform_.scale; }
	const Vector3& GetRotate() const { return transform_.rotate; }
	const Vector3& GetTranslate() const { return transform_.translate; }
	const Transform& GetTransform() const { return transform_; }

	const int32_t& GetEnableLighting() const { return enableLighting_; }
	
	Model* GetModel() const { return model_; }

private: // 静的関数

	// TransformationMatrixResourceを作成
	void CreateTransformationMatrixResource();

private:

	std::string psoName_;
	PSOManager::BlendMode blendMode_ = PSOManager::BlendMode::Normal;
	PSOManager::FillMode fillMode_ = PSOManager::FillMode::kSolid;

	// 3Dオブジェクト共通処理
	Object3dManager* object3dManager_ = nullptr;

	// モデル
	Model* model_ = nullptr;

	// カメラ
	Camera* camera_ = nullptr;

	// 座標変換行列用バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_ = nullptr;
	// バッファリソース内のデータを指すポインタ
	TransformationMatrix* transformationMatrixData_ = nullptr;

	// トランスフォーム
	Transform transform_;

	// ライティング有効無効
	int32_t enableLighting_ = true;
};


#pragma once
#include "myMath.h"

class Camera
{
public:	// メンバ関数

	Camera();

	void Initialize();

	void Update();

	void Finalize();

public:	// 外部入出力

	// セッター
	/*void SetScale(const Vector3& scale){ transform_.scale = scale; }*/
	void SetRotate(const Vector3& rotate){ transform_.rotate = rotate; }
	void SetTranslate(const Vector3& translate){ transform_.translate = translate; }
	void SetFovY(const float& fovY){ fovY_ = fovY; }
	void SetAspectRatio(const float& aspectRatio){ aspectRatio_ = aspectRatio; }
	void SetNearClip(const float& nearClip){ nearClip_ = nearClip; }
	void SetFarClip(const float& farClip){ farClip_ = farClip; }

	// ゲッター
	/*const Vector3& GetScale() const { return transform_.scale; }*/
	const Vector3& GetRotate() const { return transform_.rotate; }
	const Vector3& GetTranslate() const { return transform_.translate; }
	const Matrix4x4& GetBillboardMatrix() const { return billboardMatrix_; }	
	const float& GetFovY() const { return fovY_; }
	const float& GetAspectRatio() const { return aspectRatio_; }
	const float& GetNearClip() const { return nearClip_; }
	const float& GetFarClip() const { return farClip_; }

	const Matrix4x4& GetViewProjectionMatrix() const { return viewProjectionMatrix_; }

	const Matrix4x4 GetBillboardWorldMatrix(const Vector3& scale, const Vector3& translate) const;

private:	// メンバ変数

	Transform transform_;
	Matrix4x4 worldMatrix_;
	Matrix4x4 viewMatrix_;

	Matrix4x4 projectionMatrix_;
	float fovY_;
	float aspectRatio_;
	float nearClip_;
	float farClip_;

	Matrix4x4 viewProjectionMatrix_;

	Matrix4x4 billboardMatrix_;
	

};


#pragma once
#define _USE_MATH_DEFINES
#include "../math/myMath.h"
#include <Windows.h>

class GetKey;

/// <summary>
///  デバッグカメラ
/// </summary>
class DebugCamera
{
public:

	GetKey* getKey_;

	/// <summary>
	///  初期化
	/// </summary>
	void Initialize();

	/// <summary>
	///  更新
	/// </summary>
	void Update(GetKey getKey, Transform camera);

	Matrix4x4 GetViewMatrix() { return viewMatrix_;}


private:

	// カメラ
	Transform debugCamera_;

	// ビュー行列
	Matrix4x4 viewMatrix_ = MakeIdentity4x4();
	// 射影行列
	Matrix4x4 projectionMatrix_ = MakeIdentity4x4();

};


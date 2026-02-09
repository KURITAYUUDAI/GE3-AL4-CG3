#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

class Player;

/// <summary>
///  カメラコントローラ
/// </summary>
class CameraController 
{
	struct Rect 
	{
		float left = 0.0f;   // 左端
		float right = 1.0f;  // 右端
		float bottom = 0.0f; // 下端
		float top = 1.0f;    // 上端
	};



public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	///  更新
	/// </summary>
	void Update();

	/// <summary>
	/// デバッグ用描画
	/// </summary>
	void Draw();
	
	void Reset();
	
	void SetTarget(const Vector3& target) { target_ = target; }
	
	const Camera& GetCamera() { return camera_; }

	void SetMovableArea(Vector3 areaStart, Vector3 areaEnd) 
	{ 
		movableArea_.bottom = areaEnd.y;
		movableArea_.left = areaStart.x;
		movableArea_.top = areaStart.y;
		movableArea_.right = areaEnd.x;
	}

	void OnShake();

private:

	// カメラ
	Camera camera_;
	
	Vector3 target_;

	// 追従対象とカメラの座標の差（オフセット）
	Vector3 targetOffset_ = { 0, 0, -30.0f };

	// カメラ移動範囲
	Rect movableArea_ = { 0.0f, 100.0f, 0.0f, 100.0f };

	// カメラの目標座標
	Vector3 destinationCamera_;

	// シェイク
	float px = 4.0f;
	Vector3 shake_;
	int frame = 0;
	bool isShake_;

	// 座標補完割合
	static inline const float kInterpolationRate = 1.0f;

	// 速度掛け率
	static inline const float kVelocityBias = 0.0f;

	// 追従対象へのカメラ移動範囲
	static inline const Rect movingMargin = { -20.0f, 20.0f, -20.0f, 20.0f };

	float shakeTimer_ = 0.0f;
	static inline const float maxShakeTime = 0.5f;
};

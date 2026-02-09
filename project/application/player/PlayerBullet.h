#pragma once
#include "KamataEngine.h"
#include "myMath.h"

class Enemy;

class PlayerBullet 
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name = "model">モデル</param>
	/// <param name = "textureHandle">テクスチャハンドル</param>
	void Initialize(Model* model, const Camera* camera, const Vector3& position, const Vector3& velocity);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	void OnCollision(const Enemy* enemy);

public:	// 外部入出力

	const Vector3& GetScale() { return worldTransform_.scale_; }
	const Vector3& GetRotation() { return worldTransform_.rotation_; }
	const Vector3& GetTranslation() { return worldTransform_.translation_; };
	const Vector3& GetVelocity() { return velocity_; }
	const bool GetIsDead() { return isDead_; }

	// ワールドポジション
	const Vector3 GetWorldPosition() const;
	// AABB
	AABB GetAABB();

	void SetScale(const Vector3& scale) { worldTransform_.scale_ = scale; }
	void SetRotation(const Vector3& rotation) { worldTransform_.rotation_ = rotation; }
	void SetTranslation(const Vector3& translation) { worldTransform_.translation_ = translation; }
	void SetVelocity(const Vector3& velocity) { velocity_ = velocity; }


private:

	WorldTransform worldTransform_;

	Model* model_;
	uint32_t textureHandle_;

	const Camera* camera_;

	Vector3 size_ = {0.4f, 0.4f, 0.4f};

	const float speed_ = 2.0f;

	Vector3 direction_;

	Vector3 velocity_;

	// 寿命
	static inline const int32_t kLifeTime = 60 * 2;

	// デスタイマー
	int32_t deathTimer_ = kLifeTime;
	// デスフラグ
	bool isDead_ = false;

};

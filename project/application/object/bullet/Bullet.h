#pragma once
#include "myMath.h"
#include "object3d.h"
#include "WorldTransform.h"
#include <memory>

//class Enemy;

class Player;

class Bullet
{
public:

	enum class ID
	{
		kPlayer,
		kEnemy,
	};

public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name = "model">モデル</param>
	/// <param name = "textureHandle">テクスチャハンドル</param>
	void Initialize(const Vector3& position, const Vector3& velocity, const ID& id, const std::string& modelName);

	/// <summary>
	/// 更新
	/// </summary>
	void Update(const float& deltaTime);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	void Finalize();

	/*void OnCollision(const Enemy* enemy);*/

	void OnCollision(const Player* player);

public: // 外部入出力
	const Vector3& GetScale() { return transform_.scale; }
	const Vector3& GetRotation() { return transform_.rotate; }
	const Vector3& GetTranslation() { return transform_.translate; };
	const Vector3& GetVelocity() { return velocity_; }
	const bool GetIsDead() { return isDead_; }
	const ID& GetID() { return id_; }

	// ワールドポジション
	const Vector3 GetWorldPosition() const;
	// AABB
	AABB GetAABB();

	void SetScale(const Vector3& scale) { transform_.scale = scale; }
	void SetRotation(const Vector3& rotation) { transform_.rotate = rotation; }
	void SetTranslation(const Vector3& translation) { transform_.translate = translation; }
	void SetVelocity(const Vector3& velocity) { velocity_ = velocity; }

private:

	std::unique_ptr<Object3d> object3d_;

	Transform transform_;

	Vector3 size_ = { 0.8f, 0.8f, 0.8f };

	const float speed_ = 2.0f;

	Vector3 direction_;

	Vector3 velocity_;

	ID id_;

	// 寿命
	static inline const int32_t kLifeTime = 60 * 2;

	// デスタイマー
	int32_t deathTimer_ = kLifeTime;
	// デスフラグ
	bool isDead_ = false;
};


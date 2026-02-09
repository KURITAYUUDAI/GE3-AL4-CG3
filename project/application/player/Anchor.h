#pragma once
#include "KamataEngine.h"
#include "myMath.h"

class Player;

class Enemy;

class Anchor 
{
public:

	enum class Mode
	{
		kWait,
		kFoward,
		kBack,
		kGrapple,
		kHold,
	};

public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name = "model">モデル</param>
	/// <param name = "textureHandle">テクスチャハンドル</param>
	void Initialize(Model* model, const Camera* camera, const Player* player, const Vector3& position, const Vector3& velocity);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	void OnCollision(const Enemy* enemy);

	void Shot();

	void Shoot(const Vector3& velocity);



public: // 外部入出力
	const Vector3& GetScale() { return worldTransform_.scale_; }
	const Vector3& GetRotation() const { return worldTransform_.rotation_; }
	const Vector3& GetTranslation() { return worldTransform_.translation_; };
	const Vector3& GetVelocity() { return velocity_; }
	const bool GetIsFinished() const { return isFinished_; }
	const bool GetIsShotEnemyBullet() const { return isShotEnemyBullet_; }
	const bool GetIsShoot() const { return isShoot_; }
	const Mode GetMode() const { return mode_; }
	// ワールド行列
	const Matrix4x4 GetWorldMatrix() const { return worldTransform_.matWorld_; }
	// ワールドポジション
	const Vector3 GetWorldPosition() const;
 
	// AABB
	AABB GetAABB();

	void SetScale(const Vector3& scale) { worldTransform_.scale_ = scale; }
	void SetRotation(const Vector3& rotation) { worldTransform_.rotation_ = rotation; }
	void SetTranslation(const Vector3& translation) { worldTransform_.translation_ = translation; }
	void SetVelocity(const Vector3& velocity) { velocity_ = velocity; }
	void SetIsShotEnemyBullet(bool isShot) { isShotEnemyBullet_ = isShot; }
	void SetIsShoot(bool isShoot) { isShoot_ = isShoot; }

	void DeadHoldEnemy() { mode_ = Mode::kWait; } 

public:

	void PlaySEShot();
	void PlaySEHit();
	void PlaySEShoot();

private:
	WorldTransform worldTransform_;

	Model* model_;
	uint32_t textureHandle_;

	const Camera* camera_;

	const Player* player_;

	Vector3 size_ = {0.8f, 0.8f, 0.8f};

	const float speed_ = 0.0f;

	Vector3 direction_;

	Vector3 velocity_;

	// モード
	Mode mode_ = Mode::kWait;
	
	// 自機との距離
	float distance_;
	// 伸ばした長さ
	float length_;
	// 最大距離
	const float kMaxLength = 30.0f;
	// デルタタイム
	const float deltaTime = 0.001f;
	// 最大速度
	const float maxSpeed = 1.5f;
	// ターゲットまでのベクトル
	Vector3 toTarget;
	// 方向ベクトル
	Vector3 direction;
	// 目標速度
	Vector3 desiredVel;
	// 敵弾発射フラグ
	bool isShotEnemyBullet_;
	// 射出フラグ
	bool isShoot_;

	// 寿命
	static inline const int32_t kLifeTime = 60 * 2;

	// デスタイマー
	int32_t deathTimer_ = kLifeTime;
	//	終了フラグ
	bool isFinished_ = false;

private:

	uint32_t shotSEDataHandle_;
	uint32_t shotSEHandle_;
	bool isPlayShotSE_;

	uint32_t hitSEDataHandle_;
	uint32_t hitSEHandle_;
	bool isPlayHitSE_;

	uint32_t shootSEDataHandle_;
	uint32_t shootSEHandle_;
	bool isPlayShootSE_;
};

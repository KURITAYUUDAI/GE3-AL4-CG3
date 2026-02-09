#pragma once
#include "KamataEngine.h"
#include "myMath.h"
#include <functional>

using namespace KamataEngine;

class Enemy;

class Player;

class Anchor;

class Bullet;

class IPlayerState
{
public:
	virtual ~IPlayerState() = default;
	// 初期化
	virtual void Initialize(Player* player) = 0;
	// 更新
	virtual void Update(Player* player) = 0;
	// 描画
	virtual void Draw(Player* player) = 0;
	// 終了
	virtual void Shutdown(Player* player) = 0;
};

class Player 
{

public:

	// 振る舞い
	enum class Behavior 
	{
		kUnknown, // リクエストなし
		kRoot,    // 通常状態
		kAttack,  // 攻撃
		kAnchor,  // アンカー射出
		kCapture, // 掴み
	};


	// 角
	enum Corner 
	{
		kRightBottom, // 右下
		kLeftBottom,  // 左下
		kRightTop,    // 右上
		kLeftTop,     // 左上

		kNumCorner // 要素数
	};

	~Player();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name = "model">モデル</param>
	/// <param name = "textureHandle">テクスチャハンドル</param>
	void Initialize(Model* model, Camera* camera, const Vector3& position, Model* modelAnchor);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	void Rotate();

	void Move();

	void Shot();

	void Attack();

	void ShotAnchor();

	void OnCollision(const Enemy* enemy);

	void OnCollision(const Bullet* bullet);

public: // ビヘイビア関連

	void BehaviorRootUpdate();

	// モード変更
	void ChangeBehavior(Behavior behavior);

	Behavior GetBehaviorRequest() const { return behaviorRequest_; }
	void SetBehaviorRequest(Behavior behaviorRequest) { behaviorRequest_ = behaviorRequest; } 

public:	// 外部入出力

	/// ゲッター
	const WorldTransform& GetWorldTransform() const { return worldTransform_; }
	// トランスフォーム
	const Vector3& GetScale() const { return worldTransform_.scale_; }
	const Vector3& GetRotation() const { return worldTransform_.rotation_; }
	const Vector3& GetTranslation() const { return worldTransform_.translation_; }
	// 速度
	const Vector3& GetVelocity() const { return velocity_; }
	// ワールド行列
	const Matrix4x4 GetWorldMatrix() { return worldTransform_.matWorld_; }
	// ワールドポジション
	const Vector3 GetWorldPosition() const;
	// AABB
	AABB GetAABB();
	
	// HP
	const int& GetHitPoint() const { return hitPoint_; }
	// デスフラグ
	bool GetIsDead() const { return isDead_; } 

	/// セッター
	// トランスフォーム
	void SetScale(const Vector3& scale) { worldTransform_.scale_ = scale; }
	void SetRotation(const Vector3& rotation) { worldTransform_.rotation_ = rotation; }
	void SetTranslation(const Vector3& translation) { worldTransform_.translation_ = translation; }
	// 速度
	void SetVelocity(const Vector3 velocity) { velocity_ = velocity; }
	// アンカー射出フラグ
	void SetIsShotAnchor(bool isShot) { isShotAnchor_ = isShot; }

	Anchor* GetAnchor() { return anchor_; }

public:
	void PlaySEHit();
	void PlaySEDead();

private:	// メンバ変数

	Anchor* anchor_ = nullptr;

private:

	// ワールド変換データ
	WorldTransform worldTransform_;

	// モデル
	Model* model_ = nullptr;
	
	// カメラ
	Camera* camera_ = nullptr;

	// 移動速度
	Vector3 velocity_;

	// 移動速さ
	static inline const float kCharacterSpped = 0.2f;

	// 移動限界
	static inline const float kMoveLimitX = 13.5f;
	static inline const float kMoveLimitY = 7.0f;
	static inline const float kMoveLimitZ = 10.0f;

	// 回転速さ
	static inline const float kRotateSpeed = 1.0f / 120.0f * pi;

	// 弾の速度
	static inline const float kBulletSpeed = 1.0f;

	// 弾のクールタイム
	float coolTimer_;
	static inline const float kBulletCoolTime = 0.6f;

	// リロードの時間
	float reloadTimer_;
	static inline const float kReloadTime = 3.0f;

	// 弾数
	int bulletRemain_;
	static inline const int kMaxBullet = 10;


	// アンカー射出フラグ
	bool isShotAnchor_ = false;

	// 移動加速度
	static inline const float kAcceleration = 0.01f;
	// 移動減速度
	static inline const float kAttenuation = 0.15f;
	// 移動最大速度
	static inline const float kLimitSpeed = 0.5f;


	// キャラクターの当たり判定サイズ
	static inline const float kWidth_ = 1.8f;
	static inline const float kHeight_ = 1.8f;

	static inline const float kBlank = 0.001f;

	// HP
	int hitPoint_;
	// 最大HP
	static inline const int kMaxHitPoint = 3;

	float damageTimer_;
	const float kDamageInvincible_ = 1.0f;

	// デスフラグ
	bool isDead_ = false;

	// 現在の状態オブジェクトへのポインタ
	std::unique_ptr<IPlayerState> currentState_;

	// 振る舞い
	Behavior behavior_ = Behavior::kRoot;

	Behavior behaviorRequest_ = Behavior::kUnknown;

private:

	uint32_t hitSEDataHandle_;
	uint32_t hitSEHandle_;
	bool isPlayHitSE_;

	uint32_t deadSEDataHandle_;
	uint32_t deadSEHandle_;
	bool isPlayDeadSE_;


};

class PlayerStateRoot : public IPlayerState 
{
public:
	void Initialize(Player* player) override;
	void Update(Player* player) override;
	void Draw(Player* player) override;
	void Shutdown(Player* player) override;

private:

	// 重力加速度（下方向）
	static inline const float kGravityAcceleration = 0.01f;
	// 最大落下速度（下方向）
	static inline const float kLimitFallSpeed = 0.3f;
};

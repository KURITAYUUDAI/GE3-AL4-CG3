#pragma once
#include "KamataEngine.h"
#include "myMath.h"

using namespace KamataEngine;

class GameScene;

class MapChipField;

class Player;

class Enemy;

class PlayerBullet;

class Anchor;

class Bullet;

class IEnemyState {
public:
	virtual ~IEnemyState() = default;
	// 初期化
	virtual void Initialize(Enemy* enemy) = 0;
	// 更新
	virtual void Update(Enemy* enemy) = 0;
	// 描画
	virtual void Draw(Enemy* enemy) = 0;
	// 終了
	virtual void Shutdown(Enemy* enemy) = 0;
};

class Enemy 
{
public:

	enum class Behavior
	{
		kUnknown,	// リクエストなし
		kRoot,		// 通常
		kDead,		// デッド状態

		kGrappled,	// 掴まれ状態
		kShoot,		// 射出状態
	};

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name = "model">モデル</param>
	/// <param name = "textureHandle">テクスチャハンドル</param>
	void Initialize(Model* model, Camera* camera, const Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	void Shot(const Vector3& velocity, const float& bulletCoolTime, const bool& isNeedReload, const bool& isShot);

	void ResetBulletStatus();

public: // 衝突応答
	/*void OnCollision(const Player* player);*/

	void OnCollision(Anchor* anchor);

	void OnCollision(const Bullet* enemyBullet);

	void OnCollision(const Enemy* enemy);

public:

	void BehaviorRootUpdate();

	void BehaviorDeadUpdate();

	// モード変更
	void ChangeBehavior(Behavior behavior);

	// ビヘイビア
	Behavior GetBehavior() const { return behavior_; }
	// ビヘイビアリクエスト
	Behavior GetBehaviorRequest() const { return behaviorRequest_; }

	// ビヘイビアリクエスト
	void SetBehaviorRequest(Behavior behaviorRequest) { behaviorRequest_ = behaviorRequest; }

public:

	

	/// ゲッター
	
	// トランスフォーム
	const Vector3 GetScale() const { return worldTransform_.scale_; }
	const Vector3 GetRotation() const { return worldTransform_.rotation_; }
	const Vector3 GetTranslation() const { return worldTransform_.translation_; }
	// 死亡判定
	bool GetIsDead() const { return isDead_; }	
	// ワールド行列
	const Matrix4x4 GetWorldMatrix() { return worldTransform_.matWorld_; }
	// ワールドポジション
	Vector3 GetWorldPosition();
	// AABB
	AABB GetAABB();
	// Width
	float GetWidth() const { return kWidth_; }
	// Height
	float GetHeight() const { return kHeight_; }
	// 掴まれ時の被弾判定
	bool GetIsGrappledHit() const { return isGrappledHit_; }
	// 掴まれ判定
	bool GetIsGrappled() const { return isGrappled_; }

	// 捕まったアンカー
	Anchor* GetGrappleAnchor() { return grappleAnchor_; }
	
	bool GetIsCollisionDisabled() const { return isCollisionDisabled_; }

	/// セッター
	// ゲームシーン
	static void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }
	// プレイヤー
	void SetPlayer(Player* player) { player_ = player; }
	// トランスフォーム
	void SetScale(const Vector3& scale) { worldTransform_.scale_ = scale; }
	void SetRotation(const Vector3& rotation) { worldTransform_.rotation_ = rotation; }
	void SetTranslation(const Vector3& translation) { worldTransform_.translation_ = translation; }
	// 死亡判定
	void SetIsDead(bool isDead) { isDead_ = isDead; }
	// コリジョン無効判定
	void SetIsCollisionDisabled(bool isCollisionDisabled) { isCollisionDisabled_ = isCollisionDisabled; }
	
	// 掴まれ時の被弾判定
	void SetIsGrappledHit(bool isGrappledHit) { isGrappledHit_ = isGrappledHit; }	

public:

	static GameScene* gameScene_;

public:
	void PlaySEHit();
	void PlaySEDead();

private:

	Player* player_;

	// ワールド変換データ
	WorldTransform worldTransform_;

	// モデル
	Model* model_ = nullptr;

	// カメラ
	Camera* camera_ = nullptr;

	// 速度
	Vector3 velocity_ = {};

	// 経過時間
	float walkTimer_ = 0.0f;

	// 歩行の速さ
	static inline const float kWalkSpeed = 0.7f;

	

	// 最初の角度[度]
	static inline const float kWalkMotionAngleStrat = 25.0f;
	// 最初の角度[度]
	static inline const float kWalkMotionAngleEnd = -25.0f;
	// アニメーションの周期となる時間[秒]
	static inline const float kWalkMotionTime = 1.5f;

	// キャラクターの当たり判定サイズ
	static inline const float kWidth_ = 1.8f;
	static inline const float kHeight_ = 1.8f;

	bool isDead_ = false; // デスフラグ

	bool isCollisionDisabled_ = false; // 衝突無効フラグ

	// 現在の状態オブジェクトへのポインタ
	std::unique_ptr<IEnemyState> currentState_;

	// 振る舞い
	Behavior behavior_ = Behavior::kRoot;

	Behavior behaviorRequest_ = Behavior::kUnknown;
	
	// 掴まれフラグ
	bool isGrappled_ = false;

	// 掴まれ時のアンカーのポインタ
	Anchor* grappleAnchor_ = nullptr;

	// 掴まれ時の被弾判定
	bool isGrappledHit_ = false;

	// 弾の速度
	static inline const float kBulletSpeed = -0.5f;

	// 弾のクールタイム
	float coolTimer_;
	static inline const float kBulletCoolTime = 3.0f;

	// リロードの時間
	float reloadTimer_;
	static inline const float kReloadTime = 3.0f;

	// 弾数
	int bulletRemain_;
	static inline const int kMaxBullet = 20;

	int hitPoint_;
	const int kMaxHitPoint = 3;

	float damageTimer_;
	const float kDamageInvincible_ = 0.5f;


	bool isTarget_ = false;

private:

	
	uint32_t hitSEDataHandle_;
	uint32_t hitSEHandle_;
	bool isPlayHitSE_;

	uint32_t deadSEDataHandle_;
	uint32_t deadSEHandle_;
	bool isPlayDeadSE_;


};

class EnemyStateRoot : public IEnemyState 
{
public:
	void Initialize(Enemy* enemy) override;
	void Update(Enemy* enemy) override;
	void Draw(Enemy* enemy) override;
	void Shutdown(Enemy* enemy) override;
};

class EnemyStateDead : public IEnemyState 
{
public:
	void Initialize(Enemy* enemy) override;
	void Update(Enemy* enemy) override;
	void Draw(Enemy* enemy) override;
	void Shutdown(Enemy* enemy) override;

private:
	float animationTimer_ = 0.0f;

	float deathAnimation_ = 1.0f;
};

class EnemyStateGrappled : public IEnemyState 
{
public:
	void Initialize(Enemy* enemy) override;
	void Update(Enemy* enemy) override;
	void Draw(Enemy* enemy) override;
	void Shutdown(Enemy* enemy) override;

private:

	int hitPoint_;
	const int kMaxHitPoint = 10;

	// 弾の速度
	static inline const float kBulletSpeed = 1.5f;

	// 弾のクールタイム
	static inline const float kBulletCoolTime = 0.5f;
};

class EnemyStateShoot : public IEnemyState 
{
public:
	void Initialize(Enemy* enemy) override;
	void Update(Enemy* enemy) override;
	void Draw(Enemy* enemy) override;
	void Shutdown(Enemy* enemy) override;

private:
	
	Vector3 shootVelocity_;
	
	const float shootSpeed_ = 0.5f;

	float shootTimer_ = 0.0f;

	float maxShootTimer_ = 2.0f;
};
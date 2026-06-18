#pragma once
#include "DirectXBase.h"
#include "object3d.h"
#include "myMath.h"
#include "AIHandler.h"
#include "Model.h"
#include <memory>
#include "EnemyState.h"

#include "Collision/CollisionObserver.h"
#include "Collision/Collider.h"

class Enemy : public ICollisionObserver
{
public:

	void Initialize();
	void Update(const float& deltaTime);


	void Draw();
	void Finalize();

	void ChangeState(std::unique_ptr<IEnemyState> newState);

	void OnCollision(Collider* self, Collider* other) override;

public:	// Command

	void Move(const float& directionX, const float& directionY);
	void Decelerate();
	void Shot();
	void Attack();

public: // Command対応

	/*void MoveAvoid(const Vector3 direction, float speed);
	void SetTargetRoll(const Vector3 rollRadian);*/

public:	//外部入出力

	void SetEnvironmentTextureIndex(const uint32_t& srvIndex){ environmentTextureIndex_ = srvIndex; }

	AIHandler* GetAIHandler() { return &handler_; }

	const Vector3& GetScale() const { return transform_.scale; }
	const Vector3& GetRotate() const { return transform_.rotate; }
	const Vector3& GetTranslate() const { return transform_.translate; }
	const Transform& GetTransform() const { return transform_; }
	const Vector3& GetVelocity() const { return velocity_; }

	const float& GetDeltaTime() const { return deltaTime_; }

	const Vector3 GetWorldPosition() const;
	const Vector3 GetWorldRotate() const;

	Collider* GetCollider() { return collider_.get(); }

	// HP
	const int& GetHitPoint() const { return hitPoint_; }
	// デスフラグ
	bool GetIsDead() const { return isDead_; }

	void SetScale(const Vector3& scale) { transform_.scale = scale; }
	void SetRotate(const Vector3& rotate) { transform_.rotate = rotate; }
	void SetTranslate(const Vector3& translate) { transform_.translate = translate; }
	void SetTransform(const Transform& transform) { transform_ = transform; }
	void SetVelocity(const Vector3& velocity) { velocity_ = velocity; }

	void SetParent(WorldTransform* worldTransform);

private:

	// 入力ハンドル
	AIHandler handler_;

	// 現在の状態
	std::unique_ptr<IEnemyState> state_;

	float deltaTime_ = 0.0f;

	std::string psoName_ = "Environment";
	PSOManager::BlendMode blendMode_ = PSOManager::BlendMode::Normal;
	PSOManager::FillMode fillMode_ = PSOManager::FillMode::kSolid;

	std::unique_ptr<Object3d> object3d_;
	std::unique_ptr<Collider> collider_;

	Transform transform_;

	Vector3 velocity_ = { 0.0f, 0.0f, 0.0f };
	Vector3 maxSpeed_ = { 12.0f, 12.0f, 12.0f }; // スティック全倒し時の最高速度
	float lerpFactor_ = 0.2f;                  // 追従の滑らかさ（0〜1）
	Vector3 targetRoll_ = { 0.0f, 0.0f, 0.0f };                   // 目標のロール角Z（回転の傾き）

	// 環境テクスチャのsrvIndex
	uint32_t environmentTextureIndex_ = 0;

	bool isDraw_ = true;

	// HP
	int hitPoint_;
	// 最大HP
	static inline const int kMaxHitPoint = 3;

	float damageTimer_;
	const float kDamageInvincible_ = 1.0f;

	// デスフラグ
	bool isDead_ = false;

	float bulletSpeed_ = 10.0f;
};
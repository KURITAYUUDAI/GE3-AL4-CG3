#pragma once
#include "DirectXBase.h"
#include "object3d.h"
#include "myMath.h"
#include "AIHandler.h"
#include "Model.h"
#include <memory>
#include "EnemyState.h"

#include "collision/CollisionObserver.h"
#include "collision/Collider.h"

#include "ParticleEmitter.h"

#include "EventBus.h"

#include "GamePlayUIUtility.h"

#include "Dict_Engine/tool/effect/DissolveUtility.h"

class Enemy : public ICollisionObserver
{
public:

	void Initialize();
	void EventDispatch();
	void Update(const float& deltaTime);


	void Draw();
	void Finalize();

	void ChangeState(std::unique_ptr<IEnemyState> newState);

	void OnCollision(Collider* self, Collider* other) override;
	void Damage(int damage);

public:	// Command

	void Move(const float& directionX, const float& directionY);
	void Decelerate();
	void Shot();
	void Attack();

public: // Command対応

	/*void MoveAvoid(const Vector3 direction, float speed);
	void SetTargetRoll(const Vector3 rollRadian);*/

public:	//外部入出力

	AIHandler* GetAIHandler() { return &handler_; }

	const Vector3& GetScale() const { return transform_.scale; }
	const Vector3& GetRotate() const { return transform_.rotate; }
	const Vector3& GetTranslate() const { return transform_.translate; }
	const Transform& GetTransform() const { return transform_; }
	const Vector3& GetVelocity() const { return velocity_; }

	const float& GetDeltaTime() const { return deltaTime_; }

	const Vector3 GetWorldPosition() const;
	const Vector2 GetScreenPosition() const;
	const Vector3 GetWorldRotate() const;

	Collider* GetCollider() { return collider_.get(); }
	Collider* GetAttackCollider() { return colliderAttack_.get(); }

	// HP
	const int& GetHitPoint() const { return hitPoint_; }
	// デスフラグ
	bool GetIsDead() const { return isDead_; }

	EnemyHPGageDisplayType GetHPGageDisplayType() const { return hpGageDisplayType_; }
	int GetScreenBossPriority() const { return screenBossPriority_; }
	EnemyID GetEnemyID() const { return enemyID_; }

	const float& GetThreshold() { return dissolveParams_.threshold; }
	const Vector4& GetEdgeColor() { return dissolveParams_.edgeColor; }


	void SetScale(const Vector3& scale) { transform_.scale = scale; }
	void SetRotate(const Vector3& rotate) { transform_.rotate = rotate; }
	void SetTranslate(const Vector3& translate) { transform_.translate = translate; }
	void SetTransform(const Transform& transform) { transform_ = transform; }
	void SetVelocity(const Vector3& velocity) { velocity_ = velocity; }

	bool GetIsAttackColliderActive() const { return isAttackColliderActive_; }

	void SetEnvironmentTextureIndex(const uint32_t& srvIndex){ environmentTextureIndex_ = srvIndex; }
	void SetEventBus(EventBus* eventBus) { eventBus_ = eventBus; }

	void SetParent(WorldTransform* worldTransform);

	void SetIsHPChanged(const int isHPChanged){ isHPChanged_ = isHPChanged; }

	void SetEnemyID(EnemyID id) { enemyID_ = id; }
	void SetHPGageDisplayType(EnemyHPGageDisplayType type) { hpGageDisplayType_ = type; }
	void SetScreenBossPriority(int priority) { screenBossPriority_ = priority; }

	void SetThreshold(const float threshold) { dissolveParams_.threshold = threshold; }
	void SetEdgeColor(const Vector4 edgeColor) { dissolveParams_.edgeColor = edgeColor; }

private:

	// AI入力ハンドル
	AIHandler handler_;

	// 現在の状態
	std::unique_ptr<IEnemyState> state_;

	EnemyID enemyID_ = 0;

	EnemyHPGageDisplayType hpGageDisplayType_ =
		EnemyHPGageDisplayType::None;

	int screenBossPriority_ = 0;

	// イベント
	EventBus* eventBus_ = nullptr;
	EventSubscriber eventSubscriber_;
	std::vector<EventBus::SubscriptionID> subscriptionIDs_;

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

	float bulletSpeed_ = 15.0f;

	std::unique_ptr<Object3d> objectRightHand_;
	Transform rightHandTransform_;

	std::unique_ptr<Collider> colliderAttack_;
	bool isAttackColliderActive_ = false;

	// HP
	int hitPoint_;
	// 最大HP
	static inline const int kMaxHitPoint = 10;

	float damageTimer_;
	const float kDamageInvincible_ = 0.1f;

	// デスフラグ
	bool isDead_ = false;

	bool isHPChanged_ = false;

	Vector3 playerWorldPosition_{};

	std::unique_ptr<ParticleEmitter> slashEmitter_ = nullptr;

	DissolveParams dissolveParams_;
};
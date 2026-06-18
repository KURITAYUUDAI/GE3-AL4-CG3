#pragma once
#include "myMath.h"

class ICollisionObserver;

enum class CollisionAttribute 
{
	None,
	Player,
	Enemy,
	PlayerBullet,
	EnemyBullet,
};

class Collider
{
public:

	const Vector3& GetWorldPosition() const { return worldPosition_; }
	const float& GetRadius(){ return radius_; }
	CollisionAttribute GetAttribute() const { return attribute_; }
	
	void SetWorldPosition(const Vector3& position) { worldPosition_ = position; }
	void SetRadius(const float& radius){ radius_ = radius; }
	void SetAttribute(CollisionAttribute attribute) { attribute_ = attribute; }
	void SetOwner(ICollisionObserver* owner) { owner_ = owner; }
	
	void OnCollision(Collider* other);

private:
	float radius_ = 1.0f;
	ICollisionObserver* owner_ = nullptr;
	CollisionAttribute attribute_ = CollisionAttribute::None;

	Vector3 worldPosition_{};
};
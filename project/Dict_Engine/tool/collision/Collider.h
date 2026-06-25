#pragma once
#include "myMath.h"

class ICollisionObserver;

enum class CollisionAttribute : uint32_t
{
	None = 0,
	Player = 0b1,
	PlayerAttack = 0b1 << 1,
	Enemy = 0b1 << 2,
	EnemyAttack = 0b1 << 3,
};

class Collider
{
public:

	const Vector3& GetWorldPosition() const { return worldPosition_; }
	const float& GetRadius(){ return radius_; }
	uint32_t GetAttribute() const { return attribute_; }
	uint32_t GetMask() const {return mask_; }

	ICollisionObserver* GetOwner() { return owner_; }
	
	void SetWorldPosition(const Vector3& position) { worldPosition_ = position; }
	void SetRadius(const float& radius){ radius_ = radius; }
	void SetAttribute(CollisionAttribute attribute) { attribute_ = static_cast<uint32_t>(attribute); }
	void SetAttribute(uint32_t attribute){ attribute_ = attribute; }
	void SetMask(CollisionAttribute mask) { mask_ = ~static_cast<uint32_t>(mask); }
	void SetMask(uint32_t mask) { mask_ = mask; }
	void SetOwner(ICollisionObserver* owner) { owner_ = owner; }
	
	void OnCollision(Collider* other);

private:
	float radius_ = 1.0f;
	ICollisionObserver* owner_ = nullptr;
	uint32_t attribute_ = 0xffffffff;
	uint32_t mask_ = 0xffffffff;

	Vector3 worldPosition_{};
};
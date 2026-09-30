#pragma once

class Collider;
struct CollisionContact;

class ICollisionObserver 
{

public:
    virtual ~ICollisionObserver() = default;
    virtual void OnCollision(
		Collider* self, Collider* other, const CollisionContact& contact) = 0;

    const bool GetIsHit() { return isHit_; }

    void SetIsHit(const bool isHit) { isHit_ = isHit; }

private: 
    bool isHit_ = true;
};

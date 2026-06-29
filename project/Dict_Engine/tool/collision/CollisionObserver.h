#pragma once

class Collider;

class ICollisionObserver 
{

public:
    virtual ~ICollisionObserver() = default;
    virtual void OnCollision(Collider* self, Collider* other) = 0;

    const bool GetIsHit() { return isHit_; }

    void SetIsHit(const bool isHit) { isHit_ = isHit; }

private: 
    bool isHit_ = true;
};
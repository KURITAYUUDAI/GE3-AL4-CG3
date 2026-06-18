#pragma once

class Collider;

class ICollisionObserver 
{
public:
    virtual ~ICollisionObserver() = default;
    virtual void OnCollision(Collider* self, Collider* other) = 0;
};
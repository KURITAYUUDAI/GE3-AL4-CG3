#include "Collider.h"
#include "CollisionObserver.h"

void Collider::OnCollision(Collider* other)
{
    if (owner_) 
    {
        owner_->OnCollision(this, other);
    }
}

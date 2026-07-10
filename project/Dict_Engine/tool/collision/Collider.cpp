#include "Collider.h"
#include "CollisionObserver.h"

void Collider::UpdateWorldPosition()
{
	if (parent_)
	{
		worldPosition_ = TransformPosition(localPosition_, parent_->worldMatrix_);
	} 
    else
	{
		worldPosition_ = localPosition_;
	}
}

void Collider::OnCollision(Collider* other)
{
    if (onCollision_)
    {
		onCollision_(this, other);
        return;
    }

    if (owner_) 
    {
        owner_->OnCollision(this, other);
        return;
    }
}

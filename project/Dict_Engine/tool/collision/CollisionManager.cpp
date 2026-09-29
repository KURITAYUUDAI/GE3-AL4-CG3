#include "CollisionManager.h"
#include "Logger.h"
#include <algorithm>

namespace
{
	bool IsContinuousSphereCollision(const Collider& a, const Collider& b)
	{
		const Vector3 relativeStart =
			a.GetPreviousWorldPosition() - b.GetPreviousWorldPosition();
		const Vector3 relativeEnd =
			a.GetWorldPosition() - b.GetWorldPosition();
		const Vector3 relativeMovement = relativeEnd - relativeStart;
		const float movementLengthSquared = Dot(relativeMovement, relativeMovement);

		if (movementLengthSquared <= 0.000001f)
		{
			return false;
		}

		const float t = std::clamp(
			-Dot(relativeStart, relativeMovement) / movementLengthSquared,
			0.0f, 1.0f);
		const Vector3 closestRelativePosition =
			relativeStart + relativeMovement * t;
		const float radius = a.GetRadius() + b.GetRadius();

		return Dot(closestRelativePosition, closestRelativePosition) <= radius * radius;
	}
}

std::unique_ptr<CollisionManager> CollisionManager::instance_ = nullptr;

CollisionManager* CollisionManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = std::make_unique<CollisionManager>(ConstructorKey());
	}
	return instance_.get();
}

void CollisionManager::Finalize()
{
	Clear();
	instance_.reset();
}

void CollisionManager::AddCollider(Collider* collider)
{
	colliders_.push_back(collider);
}

void CollisionManager::Clear()
{
	colliders_.clear();
}

void CollisionManager::CheckAllCollisions()
{
    for (auto itrA = colliders_.begin(); itrA != colliders_.end(); ++itrA) 
    {
        auto itrB = itrA;
        ++itrB;

        for (; itrB != colliders_.end(); ++itrB) 
        {
            Collider* a = *itrA;
            Collider* b = *itrB;

			if ((a->GetAttribute() & b->GetMask()) == 0 ||
				(b->GetAttribute() & a->GetMask()) == 0)
			{
				continue;
			}

            bool isColliding = false;
            if (a->GetShape() == ColliderShape::Sphere &&
                b->GetShape() == ColliderShape::Sphere)
            {
                const Sphere sphereA{ a->GetWorldPosition(), a->GetRadius() };
                const Sphere sphereB{ b->GetWorldPosition(), b->GetRadius() };
                isColliding = IsCollision(sphereA, sphereB);

				if (!isColliding &&
					(a->IsContinuousCollisionEnabled() || b->IsContinuousCollisionEnabled()))
				{
					isColliding = IsContinuousSphereCollision(*a, *b);
				}
            }
            else if (a->GetShape() == ColliderShape::AABB &&
                     b->GetShape() == ColliderShape::AABB)
            {
                isColliding = IsCollision(a->GetAABB(), b->GetAABB());
            }
            else if (a->GetShape() == ColliderShape::AABB)
            {
                const Sphere sphereB{ b->GetWorldPosition(), b->GetRadius() };
                isColliding = IsCollision(a->GetAABB(), sphereB);
            }
            else
            {
                const Sphere sphereA{ a->GetWorldPosition(), a->GetRadius() };
                isColliding = IsCollision(b->GetAABB(), sphereA);
            }

            if (isColliding)
            {
                a->OnCollision(b);
                b->OnCollision(a);
            }
        }
    }
}


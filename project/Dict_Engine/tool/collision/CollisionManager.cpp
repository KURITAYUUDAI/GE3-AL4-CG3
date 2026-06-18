#include "CollisionManager.h"
#include "Logger.h"

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

			if (!CanCheckCollision(a, b))
			{
				continue;
			}

            Vector3 posA = a->GetWorldPosition();
            Vector3 posB = b->GetWorldPosition();

            float distance = Length(posB - posA);
            float radiusSum = a->GetRadius() + b->GetRadius();

            if (distance <= radiusSum) 
            {
                a->OnCollision(b);
                b->OnCollision(a);
            }
        }
    }
}

bool IsPair(
		CollisionAttribute a,
		CollisionAttribute b,
		CollisionAttribute targetA,
		CollisionAttribute targetB)
{
	return
		(a == targetA && b == targetB) ||
		(a == targetB && b == targetA);
};

bool CollisionManager::CanCheckCollision(Collider* a, Collider* b)
{
	CollisionAttribute attrA = a->GetAttribute();
	CollisionAttribute attrB = b->GetAttribute();

	if (IsPair(attrA, attrB, CollisionAttribute::PlayerBullet, CollisionAttribute::PlayerBullet))
	{
		return false;
	}

	if (IsPair(attrA, attrB, CollisionAttribute::EnemyBullet, CollisionAttribute::EnemyBullet))
	{
		return false;
	}

	if (IsPair(attrA, attrB, CollisionAttribute::Player, CollisionAttribute::PlayerBullet))
	{
		return false;
	}

	if (IsPair(attrA, attrB, CollisionAttribute::Enemy, CollisionAttribute::EnemyBullet))
	{
		return false;
	}


	return true;
}
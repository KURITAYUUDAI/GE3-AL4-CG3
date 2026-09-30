#define NOMINMAX
#include "CollisionManager.h"
#include "Logger.h"
#include <algorithm>
#include <cmath>

namespace
{
	constexpr float kContactEpsilon = 0.000001f;

	struct CollisionResult
	{
		bool isColliding = false;
		CollisionContact contact{};
	};

	Vector3 SafeNormalize(const Vector3& value)
	{
		return Dot(value, value) > kContactEpsilon
			? Normalize(value)
			: Vector3{ 1.0f, 0.0f, 0.0f };
	}

	CollisionContact MakeSphereContact(
		const Vector3& centerA, float radiusA,
		const Vector3& centerB, float radiusB,
		float timeOfImpact = 1.0f, bool isContinuous = false)
	{
		const Vector3 normal = SafeNormalize(centerB - centerA);
		const Vector3 surfaceA = centerA + normal * radiusA;
		const Vector3 surfaceB = centerB - normal * radiusB;
		return CollisionContact{
			.position = (surfaceA + surfaceB) * 0.5f,
			.normal = normal,
			.timeOfImpact = timeOfImpact,
			.isContinuous = isContinuous,
		};
	}

	CollisionResult CheckContinuousSphereCollision(
		const Collider& a, const Collider& b)
	{
		const Vector3 startA = a.GetPreviousWorldPosition();
		const Vector3 startB = b.GetPreviousWorldPosition();
		const Vector3 movementA = a.GetWorldPosition() - startA;
		const Vector3 movementB = b.GetWorldPosition() - startB;
		const Vector3 relativeStart = startA - startB;
		const Vector3 relativeMovement = movementA - movementB;
		const float radius = a.GetRadius() + b.GetRadius();
		const float quadraticA = Dot(relativeMovement, relativeMovement);
		const float quadraticC = Dot(relativeStart, relativeStart) - radius * radius;

		if (quadraticA <= kContactEpsilon)
		{
			return {};
		}

		float timeOfImpact = 0.0f;
		if (quadraticC > 0.0f)
		{
			const float quadraticB = 2.0f * Dot(relativeStart, relativeMovement);
			const float discriminant =
				quadraticB * quadraticB - 4.0f * quadraticA * quadraticC;
			if (discriminant < 0.0f || !std::isfinite(discriminant))
			{
				return {};
			}

			timeOfImpact =
				(-quadraticB - std::sqrt(discriminant)) / (2.0f * quadraticA);
			if (timeOfImpact < 0.0f || timeOfImpact > 1.0f ||
				!std::isfinite(timeOfImpact))
			{
				return {};
			}
		}

		const Vector3 centerA = startA + movementA * timeOfImpact;
		const Vector3 centerB = startB + movementB * timeOfImpact;
		return CollisionResult{
			.isColliding = true,
			.contact = MakeSphereContact(
				centerA, a.GetRadius(), centerB, b.GetRadius(),
				timeOfImpact, true),
		};
	}

	CollisionContact MakeAABBSphereContact(
		const AABB& box, const Vector3& sphereCenter)
	{
		Vector3 point{
			std::clamp(sphereCenter.x, box.min.x, box.max.x),
			std::clamp(sphereCenter.y, box.min.y, box.max.y),
			std::clamp(sphereCenter.z, box.min.z, box.max.z),
		};
		const Vector3 boxCenter = (box.min + box.max) * 0.5f;
		Vector3 normal = sphereCenter - point;

		// Sphere中心がAABB内部の場合は、最も近い面を接触点として使う。
		if (Dot(normal, normal) <= kContactEpsilon)
		{
			const float distances[6] = {
				sphereCenter.x - box.min.x, box.max.x - sphereCenter.x,
				sphereCenter.y - box.min.y, box.max.y - sphereCenter.y,
				sphereCenter.z - box.min.z, box.max.z - sphereCenter.z,
			};
			const size_t face = static_cast<size_t>(
				std::min_element(distances, distances + 6) - distances);
			switch (face)
			{
			case 0: point.x = box.min.x; break;
			case 1: point.x = box.max.x; break;
			case 2: point.y = box.min.y; break;
			case 3: point.y = box.max.y; break;
			case 4: point.z = box.min.z; break;
			default: point.z = box.max.z; break;
			}
			normal = sphereCenter - boxCenter;
		}

		return CollisionContact{
			.position = point,
			.normal = SafeNormalize(normal),
		};
	}

	CollisionContact MakeAABBContact(const AABB& a, const AABB& b)
	{
		const Vector3 overlapMin{
			std::max(a.min.x, b.min.x),
			std::max(a.min.y, b.min.y),
			std::max(a.min.z, b.min.z),
		};
		const Vector3 overlapMax{
			std::min(a.max.x, b.max.x),
			std::min(a.max.y, b.max.y),
			std::min(a.max.z, b.max.z),
		};
		const Vector3 centerA = (a.min + a.max) * 0.5f;
		const Vector3 centerB = (b.min + b.max) * 0.5f;
		return CollisionContact{
			.position = (overlapMin + overlapMax) * 0.5f,
			.normal = SafeNormalize(centerB - centerA),
		};
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

			CollisionResult result{};
            if (a->GetShape() == ColliderShape::Sphere &&
                b->GetShape() == ColliderShape::Sphere)
            {
                const Sphere sphereA{ a->GetWorldPosition(), a->GetRadius() };
                const Sphere sphereB{ b->GetWorldPosition(), b->GetRadius() };
				result.isColliding = IsCollision(sphereA, sphereB);
				if (result.isColliding)
				{
					result.contact = MakeSphereContact(
						sphereA.center, sphereA.radius, sphereB.center, sphereB.radius);
				}

				if (a->IsContinuousCollisionEnabled() || b->IsContinuousCollisionEnabled())
				{
					const CollisionResult continuousResult =
						CheckContinuousSphereCollision(*a, *b);
					if (continuousResult.isColliding)
					{
						result = continuousResult;
					}
				}
            }
            else if (a->GetShape() == ColliderShape::AABB &&
                     b->GetShape() == ColliderShape::AABB)
            {
				const AABB boxA = a->GetAABB();
				const AABB boxB = b->GetAABB();
				result.isColliding = IsCollision(boxA, boxB);
				if (result.isColliding)
				{
					result.contact = MakeAABBContact(boxA, boxB);
				}
            }
            else if (a->GetShape() == ColliderShape::AABB)
            {
				const AABB boxA = a->GetAABB();
                const Sphere sphereB{ b->GetWorldPosition(), b->GetRadius() };
				result.isColliding = IsCollision(boxA, sphereB);
				if (result.isColliding)
				{
					result.contact = MakeAABBSphereContact(boxA, sphereB.center);
				}
            }
            else
            {
                const Sphere sphereA{ a->GetWorldPosition(), a->GetRadius() };
				const AABB boxB = b->GetAABB();
				result.isColliding = IsCollision(boxB, sphereA);
				if (result.isColliding)
				{
					result.contact = MakeAABBSphereContact(boxB, sphereA.center);
					result.contact.normal = result.contact.normal * -1.0f;
				}
            }

			if (result.isColliding)
            {
				a->OnCollision(b, result.contact);
				CollisionContact inverseContact = result.contact;
				inverseContact.normal = inverseContact.normal * -1.0f;
				b->OnCollision(a, inverseContact);
            }
        }
    }
}


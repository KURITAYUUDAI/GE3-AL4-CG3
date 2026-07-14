#include "AIHandler.h"

Vector2 AIHandler::GetDirection()
{
	return Vector2();
}

bool AIHandler::IsShot(const float& deltaTime)
{
	shotTimer_ += deltaTime;

	if (shotTimer_ >= shotDuration_)
	{
		shotTimer_ = 0.0f;
		return true;
	}

	return false;
}

bool AIHandler::IsAttack(const float& deltaTime)
{
	attackTimer_ += deltaTime;
	if (attackTimer_ >= attackDuration_)
	{
		attackTimer_ = 0.0f;
		return true;
	}
	return false;
}



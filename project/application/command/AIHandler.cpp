#include "AIHandler.h"

Vector2 AIHandler::GetDirection()
{
	return Vector2();
}

bool AIHandler::IsShot(const float& deltaTime)
{
	timer_ += deltaTime;

	if (timer_ >= duration_)
	{
		timer_ = 0.0f;
		return true;
	}

	return false;
}



#pragma once
#include "myMath.h"
#include <cmath>
#include <string>
#include <vector>
#include <unordered_map>

class AIHandler
{
public:

	Vector2 GetDirection();
	bool IsShot(const float& deltaTime);

private:

	float timer_ = 0.0f;
	float duration_ = 5.0f;
};


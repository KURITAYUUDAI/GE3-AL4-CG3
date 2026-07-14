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
	bool IsAttack(const float& deltaTime);

private:

	float shotTimer_ = 0.0f;
	float shotDuration_ = 5.0f;

	float attackTimer_ = 0.0f;
	float attackDuration_ = 7.0f;
};


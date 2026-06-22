#pragma once
#include "myMath.h"

struct DissolveParams
{
	float threshold = 0.0f;
	float padding[3];
	Vector4 edgeColor = { 1.0f, 0.4f, 0.4f, 1.0f };
};
#pragma once
#include "myMath.h"

class Camera;

class ICameraController
{
public:

	virtual void Initialize() = 0;
	virtual void Update(Camera* mainCamera, const float& deltaTime) = 0;
	virtual void Finalize() = 0;
	virtual void DrawDebugUI() = 0;

};


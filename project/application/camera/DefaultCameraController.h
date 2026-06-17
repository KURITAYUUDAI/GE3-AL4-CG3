#pragma once
#include "ICameraController.h"

class DefaultCameraController : public ICameraController
{
public:

	void Initialize() override;
	void Update(Camera* mainCamera, const float& deltaTime) override;
	void Finalize() override;
	void DrawDebugUI(const Camera* mainCamera, bool& isDebugCamera) override;

};


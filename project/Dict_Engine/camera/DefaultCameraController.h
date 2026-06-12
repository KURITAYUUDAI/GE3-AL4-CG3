#pragma once
#include "ICameraController.h"
#include "WorldTransform.h"

class DefaultCameraController : public ICameraController
{
public:

	void Initialize() override;
	void Update(Camera* mainCamera, const float& deltaTime) override;
	void Finalize() override;
	void DrawDebugUI() override;	

public:

	const Vector3& GetRotate(){ return worldTransform_.rotate_; }
	const Vector3& GetTranslate(){ return worldTransform_.translate_; }
	WorldTransform* GetWorldTransform() { return &worldTransform_; }

	void SetRotate(const Vector3& rotate){ worldTransform_.rotate_ = rotate; }
	void SetTranslate(const Vector3& translate){ worldTransform_.translate_ = translate; }

private:

	WorldTransform worldTransform_;

};


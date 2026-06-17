#pragma once
#include "ICameraController.h"
#include "SplineCurve.h"

class RailCameraController :public ICameraController
{
public:

	void Initialize() override;
	void Update(Camera* mainCamera, const float& deltaTime) override;
	void Finalize() override;
	void DrawDebugUI(const Camera* mainCamera) override;

	void BuildLenghthTable();
	float GetTFromDistance(float distance);

private:

	std::vector<Vector3> controlPoints_;

	Vector3 target_ = {0.0f, 0.0f, 0.0f};

	std::vector<SplineSample> lengthTable_; // 距離逆引き用テーブル
	float totalLength_ = 0.0f;              // レール全体の総延長（長さ）

	float currentDistance_ = 0.0f;          // 【重要】progress_の代わりに「進んだ距離(メートル)」で管理する
	float moveSpeed_ = 5.0f;                // 移動速度 (1秒間に進むメートル数)

	float targetRange_ = 0.5f;
	
	float heightOffset_ = 1.0f;
};


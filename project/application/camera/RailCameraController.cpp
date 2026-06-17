#define NOMINMAX
#include "RailCameraController.h"
#include "Camera.h"
#include "CameraManager.h"
#include "DebugDrawManager.h"
#include "SplineCurve.h"
#include "ImGuiManager.h"

void RailCameraController::Initialize()
{
	controlPoints_ =
	{
		{  0,  0,  0 },
		{-10, 10,  0 },
		{-10, 15,  0 },
		{ 20, 15, 15 },
		{ 20,  0,  0 },
		{ 10,  0,-15 },
	};


	moveSpeed_ = 1.0f;
	currentDistance_ = 0.0f;

	worldTransform_.Initialize();
	worldTransform_.scale_ = { 1.0f, 1.0f, 1.0f };
	worldTransform_.SetRotate({ 0.0f, 0.0f, 0.0f });

	BuildLenghthTable();

	worldTransform_.translate_ = GetLoopSplinePosition(controlPoints_, GetTFromDistance(currentDistance_));
	target_ = GetLoopSplinePosition(controlPoints_, GetTFromDistance(currentDistance_ + targetRange_));
}

void RailCameraController::Update(Camera* mainCamera, const float& deltaTime)
{
#ifdef _DEBUG

	ImGui::Begin("ReilCamera");

	ImGui::DragFloat2("Offset", &offset_.x, 0.1f);
	ImGui::DragFloat("MoveSpeed", &moveSpeed_, 0.1f);
	ImGui::InputFloat3("RailPos", &railPos_.x, "%.3f", ImGuiInputTextFlags_ReadOnly);

	ImGui::End();

#endif

	if (!mainCamera) return;

	// 1. 毎フレーム「時間 × 速度」で実際の移動距離を加算する
	currentDistance_ += moveSpeed_ * deltaTime;
	if (currentDistance_ > totalLength_)
	{
		currentDistance_ -= totalLength_; // ループ（外周を超えたら先頭へ戻る）
	}

	// 2. 現在の距離に対応する「正しいt」を逆引きする
	float equalT = GetTFromDistance(currentDistance_);
	railPos_ = GetLoopSplinePosition(controlPoints_, equalT);

	// 3. ターゲット（注視点）の位置も「現在の距離 + 〇〇メートル先」から取得する
	float targetDistance = currentDistance_ + targetRange_;
	float targetT = GetTFromDistance(targetDistance);
	target_ = GetLoopSplinePosition(controlPoints_, targetT);

	// --- 以降のベクトル計算やカメラ設定はそのまま流用 ---
	Vector3 forward = target_ - railPos_;
	forward = Normalize(forward);

	Vector3 upIndex = { 0.0f, 1.0f, 0.0f };
	Vector3 xAxis = Cross(upIndex, forward);
	float xLength = std::sqrt(xAxis.x * xAxis.x + xAxis.y * xAxis.y + xAxis.z * xAxis.z);
	if (xLength > 0.001f) { xAxis.x /= xLength; xAxis.y /= xLength; xAxis.z /= xLength; } else { xAxis = { 1.0f, 0.0f, 0.0f }; }

	Vector3 localUp = Cross(forward, xAxis);

	Vector3 eyePos = {
		railPos_.x + xAxis.x * offset_.x + localUp.x * offset_.y,
		railPos_.y + xAxis.y * offset_.x + localUp.y * offset_.y,
		railPos_.z + xAxis.z * offset_.x + localUp.z * offset_.y
	};

	// (前回提案した行列による回転制御への移行を推奨しますが、一旦既存のオイラー角のままでも動作します)
	Vector3 rotate = { 0.0f, 0.0f, 0.0f };
	rotate.y = std::atan2(forward.x, forward.z);
	float xzlength = Length(Vector2{ forward.x, forward.z });
	rotate.x = std::atan2(-forward.y, xzlength);

	worldTransform_.translate_ = eyePos;
	worldTransform_.SetRotate(rotate);
	worldTransform_.UpdateMatrix();

	mainCamera->SetRotate(worldTransform_.GetRotate());
	mainCamera->SetTranslate(worldTransform_.translate_);
}

void RailCameraController::Finalize()
{

}

void RailCameraController::DrawDebugUI(const Camera* mainCamera, bool& isDebugCamera)
{
	if (isDebugCamera)
	{
		DebugDrawManager::GetInstance()->AddSphere(GetWorldPosition(), 0.2f, { 0.0f, 1.0f, 0.0f, 1.0f }, 12);
		DebugDrawManager::GetInstance()->AddLine(GetWorldPosition(), railPos_, {0.0f, 0.0f, 1.0f, 1.0f});
		DebugDrawManager::GetInstance()->AddSphere(railPos_, 0.2f, { 1.0f, 1.0f, 0.0f, 1.0f }, 12);
		DebugDrawManager::GetInstance()->AddSphere(target_, 0.2f, { 0.0f, 1.0f, 1.0f, 1.0f }, 12);
	}
	
	DebugDrawManager::GetInstance()->AddLoopSpline(controlPoints_, { 1.0f, 0.0f, 0.0f, 1.0f }, 300);
}

void RailCameraController::BuildLenghthTable()
{
	lengthTable_.clear();
	totalLength_ = 0.0f;

	// 分割数（制御点の数やレールの長さに応じて 200~500 程度に調整）
	const uint32_t sampleCount = 400;

	// 始点 (t = 0.0) の座標
	Vector3 previousP = GetLoopSplinePosition(controlPoints_, 0.0f);
	lengthTable_.push_back({ 0.0f, 0.0f });

	for (uint32_t i = 1; i <= sampleCount; ++i)
	{
		float t = static_cast<float>(i) / static_cast<float>(sampleCount);
		Vector3 currentP = GetLoopSplinePosition(controlPoints_, t);

		// 前のサンプリング点からの微小距離を計算
		float diffX = currentP.x - previousP.x;
		float diffY = currentP.y - previousP.y;
		float diffZ = currentP.z - previousP.z;
		float segmentLength = std::sqrt(diffX * diffX + diffY * diffY + diffZ * diffZ);

		// 累積距離を更新してテーブルに登録
		totalLength_ += segmentLength;
		lengthTable_.push_back({ totalLength_, t });

		previousP = currentP;
	}
}

float RailCameraController::GetTFromDistance(float distance)
{
	if (lengthTable_.empty()) return 0.0f;

	// ループ対応: distance が全長の範囲内（0.0f ~ totalLength_）に収まるように循環させる
	if (distance < 0.0f) distance = 0.0f;
	if (distance > totalLength_)
	{
		distance = std::fmod(distance, totalLength_);
	}

	// 二分探索で distance 以上の最初の要素（It）を検索
	auto it = std::lower_bound(lengthTable_.begin(), lengthTable_.end(), distance,
		[](const SplineSample& sample, float dist){
			return sample.distance < dist;
		});

	if (it == lengthTable_.begin()) return lengthTable_.front().t;
	if (it == lengthTable_.end()) return lengthTable_.back().t;

	// 見つかった位置とその手前の要素で線形補間を行う
	const auto& prevSample = *(it - 1);
	const auto& nextSample = *it;

	float denom = nextSample.distance - prevSample.distance;
	if (denom < 0.0001f) return prevSample.t;

	// 距離の割合を算出し、tを補間
	float factor = (distance - prevSample.distance) / denom;
	return prevSample.t + (nextSample.t - prevSample.t) * factor;
}

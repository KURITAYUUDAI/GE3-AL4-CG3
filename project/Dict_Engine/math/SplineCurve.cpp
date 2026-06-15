#include "SplineCurve.h"

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t)
{
	return
	{
		v2.x * t + v1.x * (1.0f - t),
		v2.y * t + v1.y * (1.0f - t),
		v2.z * t + v1.z * (1.0f - t)
	};
}

Vector3 CatmullRom(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t)
{
	return 0.5f * (
	   (2.0f * p1) +
	   (-p0 + p2) * t +
	   (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t * t +
	   (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t * t * t
   );
}

void DrawBezier(const Vector3& controlPoint0, const Vector3& controlPoint1, 
	const Vector3& controlPoint2, const uint32_t segments, const Vector4& color)
{
	auto* debugManager = DebugDrawManager::GetInstance();
	Vector3 previousP = controlPoint0;

	for (uint32_t i = 1; i < segments; ++i)
	{
		float t = static_cast<float>(i) / static_cast<float>(segments);
		Vector3 p0p1 = Lerp(controlPoint0, controlPoint1, t);
		Vector3 p1p2 = Lerp(controlPoint1, controlPoint2, t);
		Vector3 currentP = Lerp(p0p1, p1p2, t);

		/*Vector3 nextP0P1 = Lerp(controlPoint0, controlPoint1, t + 1.0f / 32.0f);
		Vector3 nextP1P2 = Lerp(controlPoint1, controlPoint2, t + 1.0f / 32.0f);
		Vector3 nextP = Lerp(nextP0P1, nextP1P2, t + 1.0f / 32.0f);*/
		debugManager->AddLine(previousP, currentP, color);
	}

	Sphere controlSphere[3];
	controlSphere[0] = { controlPoint0, 0.01f };
	controlSphere[1] = { controlPoint1, 0.01f };
	controlSphere[2] = { controlPoint2, 0.01f };

	for (uint32_t i = 0; i < 3; ++i)
	{
		debugManager->AddSphere(controlSphere[i].center, controlSphere[i].radius, { 1.0f, 1.0f, 1.0f, 1.0f });
	}

}

//void DrawSpline(const Vector3& controlPoint0, const Vector3& controlPoint1, 
//	const Vector3& controlPoint2, const Vector3& controlPoint3, 
//	const uint32_t segments, const Vector4& color)
//{
//	auto* debugManager = DebugDrawManager::GetInstance();
//
//	for (uint32_t i = 0; i < segments; ++i)
//	{
//		float t = static_cast<float>(i) / static_cast<float>(segments);
//		float nextT = static_cast<float>(i + 1) / static_cast<float>(segments);
//
//		// 現在の点の座標
//		Vector3 p = CatmullRom(controlPoint0, controlPoint1, controlPoint2, controlPoint3, t);
//		// 次の点の座標
//		Vector3 nextP = CatmullRom(controlPoint0, controlPoint1, controlPoint2, controlPoint3, nextT);
//
//		debugManager->AddLine(p, nextP, color);
//	}
//
//	Vector3 controlPoints[4] = { controlPoint0, controlPoint1, controlPoint2, controlPoint3 };
//	for (uint32_t i = 0; i < 4; ++i)
//	{
//		// 通過する点（P1, P2）は緑、通過しない点（P0, P3）は白、のように色を変えるとさらに見やすくなります！
//		Vector4 sphereColor = (i == 1 || i == 2) ? Vector4{ 0.0f, 1.0f, 0.0f, 1.0f } : Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
//		debugManager->AddSphere(controlPoints[i], 0.01f, sphereColor);
//	}
//}

void DrawSpline(const std::vector<Vector3>& controlPoints, const Vector4& color, 
	uint32_t segments)
{
	// 制御点が少なすぎる場合は描画できない
	if (controlPoints.size() < 2 || segments <= 0) return;

	auto* debugManager = DebugDrawManager::GetInstance();

	// 制御点の総数
	size_t numPoints = controlPoints.size();
	size_t numSections = numPoints - 1;

	Vector3 previousP = controlPoints[0];

	for (uint32_t i = 1; i <= segments; ++i)
	{
		// 曲線全体を通した進捗率 (0.0 から 1.0)
		float globalT = static_cast<float>(i) / static_cast<float>(segments);

		// globalT から「現在どの区間にいるか」のインデックスを計算する
		// 例: globalT = 0.5 で 4区間ある場合、 0.5 * 4 = 2.0 -> sectionIndexは2番目
		float sectionProgress = globalT * static_cast<float>(numSections);
		size_t sectionIndex = static_cast<size_t>(sectionProgress);

		// globalT = 1.0 の時など、配列外参照を防ぐためのガード
		if (sectionIndex >= numSections)
		{
			sectionIndex = numSections - 1;
		}

		// その区間内だけのローカルな進捗率 (0.0 から 1.0) に変換
		float localT = sectionProgress - static_cast<float>(sectionIndex);

		// 4つの制御点 (P0, P1, P2, P3) をクランプ処理を交えて取得
		Vector3 p0 = (sectionIndex == 0) ? controlPoints[0] : controlPoints[sectionIndex - 1];
		Vector3 p1 = controlPoints[sectionIndex];
		Vector3 p2 = controlPoints[sectionIndex + 1];
		Vector3 p3 = (sectionIndex + 2 >= numPoints) ? controlPoints[numPoints - 1] : controlPoints[sectionIndex + 2];

		// 現在の補間座標を計算
		Vector3 currentP = CatmullRom(p0, p1, p2, p3, localT);

		// 前回の位置から現在の位置へ線を引く
		debugManager->AddLine(previousP, currentP, color);

		// 次のステップのために現在の座標を保存
		previousP = currentP;
	}

	for (size_t i = 0; i < numPoints; ++i)
	{
		debugManager->AddSphere(controlPoints[i], 0.02f, { 1.0f, 1.0f, 1.0f, 1.0f });
	}
}

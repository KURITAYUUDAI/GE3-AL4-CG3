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

Vector3 GetSplinePosition(const std::vector<Vector3>& controlPoints, float globalT)
{
    if (controlPoints.empty()) return Vector3(0, 0, 0);
    if (controlPoints.size() == 1) return controlPoints[0];

    // globalTを 0.0f ~ 1.0f の範囲にクランプ
    globalT = std::clamp(globalT, 0.0f, 1.0f);

    size_t numSections = controlPoints.size() - 1;
    float sectionProgress = globalT * static_cast<float>(numSections);
    size_t sectionIndex = static_cast<size_t>(sectionProgress);

    if (sectionIndex >= numSections)
    {
        sectionIndex = numSections - 1;
    }

    float localT = sectionProgress - static_cast<float>(sectionIndex);

    Vector3 p0 = (sectionIndex == 0) ? controlPoints[0] : controlPoints[sectionIndex - 1];
    Vector3 p1 = controlPoints[sectionIndex];
    Vector3 p2 = controlPoints[sectionIndex + 1];
    Vector3 p3 = (sectionIndex + 2 >= controlPoints.size()) ? controlPoints[controlPoints.size() - 1] : controlPoints[sectionIndex + 2];

    return CatmullRom(p0, p1, p2, p3, localT);
}

Vector3 GetLoopSplinePosition(const std::vector<Vector3>& controlPoints, float globalT)
{
    if (controlPoints.empty()) return Vector3(0, 0, 0);
    if (controlPoints.size() == 1) return controlPoints[0];

    // globalTを 0.0f ~ 1.0f の範囲にクランプ
    globalT = std::clamp(globalT, 0.0f, 1.0f);

    size_t numPoints = controlPoints.size();
    // 変更点: ループするため、区間数は制御点の総数（N）と同じになる
    size_t numSections = numPoints;

    float sectionProgress = globalT * static_cast<float>(numSections);
    size_t sectionIndex = static_cast<size_t>(sectionProgress);

    float localT = 0.0f;
    if (sectionIndex >= numSections)
    {
        sectionIndex = numSections - 1;
        localT = 1.0f; // globalT = 1.0f の時は最後の区間の終端にする
    } else
    {
        localT = sectionProgress - static_cast<float>(sectionIndex);
    }

    // 変更点: 剰余算 (%) を使ってインデックスを循環させる
    // 負のインデックス対策として (sectionIndex + numPoints - 1) を行う
    size_t idx0 = (sectionIndex + numPoints - 1) % numPoints;
    size_t idx1 = sectionIndex;
    size_t idx2 = (sectionIndex + 1) % numPoints;
    size_t idx3 = (sectionIndex + 2) % numPoints;

    Vector3 p0 = controlPoints[idx0];
    Vector3 p1 = controlPoints[idx1];
    Vector3 p2 = controlPoints[idx2];
    Vector3 p3 = controlPoints[idx3];

    return CatmullRom(p0, p1, p2, p3, localT);
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

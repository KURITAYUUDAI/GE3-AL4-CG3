#pragma once
#include "myMath.h"
#include "DebugDrawManager.h"

struct SplineSample
{
    float distance; // 始点からの累積距離
    float t;        // その距離に対応する全体の進捗率 globalT (0.0f ~ 1.0f)
};

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);
Vector3 CatmullRom(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p4, float t);

Vector3 GetSplinePosition(const std::vector<Vector3>& controlPoints, float globalT);

Vector3 GetLoopSplinePosition(const std::vector<Vector3>& controlPoints, float globalT);
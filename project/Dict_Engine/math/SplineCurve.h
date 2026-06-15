#pragma once
#include "myMath.h"
#include "DebugDrawManager.h"

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);

Vector3 CatmullRom(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p4, float t);



//void DrawSpline(const Vector3& controlPoint0, const Vector3& controlPoint1,
//	const Vector3& controlPoint2, const Vector3& controlPoint3, const uint32_t segments, const Vector4& color);

void DrawBezier(const Vector3& controlPoint0, const Vector3& controlPoint1,
	const Vector3& controlPoint2, const uint32_t segments, const Vector4& color);

void DrawSpline(const std::vector<Vector3>& controlPoints, const Vector4& color,
	uint32_t segments);

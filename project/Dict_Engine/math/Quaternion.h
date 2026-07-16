#pragma once
#include <cmath>
#include <algorithm>
#include <stdlib.h>
#include "myMath.h"

struct Quaternion
{
	float x, y, z, w;
};

struct QuaternionTransform
{
	Vector3 scale;
	Quaternion rotate;
	Vector3 translate;
};


// Quaternionの積
Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs);

// 単位Quaternionを返す
Quaternion IdentityQuaternion();

// 共役Quaternionを返す
Quaternion Conjugate(const Quaternion& quaternion);

// Quaternionのnormを返す
float Norm(const Quaternion& quaternion);

// 正規化したQuaternionを返す
Quaternion Normalize(const Quaternion& quaternion);

// 逆Quaternionを返す
Quaternion Inverse(const Quaternion& quaternion);

inline Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs)
{
	return
	{
		lhs.x + rhs.x,
		lhs.y + rhs.y,
		lhs.z + rhs.z,
		lhs.w + rhs.w,
	};
}

inline Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs)
{
	return Multiply(lhs, rhs);
}

inline Quaternion operator*(float scalar, const Quaternion& quaternion)
{
	return
	{
		scalar * quaternion.x,
		scalar * quaternion.y,
		scalar * quaternion.z,
		scalar * quaternion.w
	};
}

inline Quaternion operator*(const Quaternion& quaternion, float scalar)
{
	return scalar * quaternion;
}

// 任意軸回転を表すQuaternionの生成
Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle);

// ベクトルをQuaternionで回転させた結果のベクトルを求める
Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion);

// Quaternionから回転行列を求める
Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion);

float Dot(const Quaternion& q1, const Quaternion& q2);

// 球面線形補間　使う前に必ず正規化を掛けること
Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);

Quaternion MakeFromEuler(const Vector3& euler);

// YXZ回転
Vector3 MakeToEuler(Quaternion q);

// YXZ回転
Quaternion MakeFromMatrix(const Matrix4x4& m);

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Quaternion& rotate, const Vector3& translate);

//Quaternion rotation0 = MakeRotateAxisAngleQuaternion({ 0.71f, 0.71f, 0.0f }, 0.3f);
//
//Quaternion rotation1 = MakeRotateAxisAngleQuaternion({ 0.71f, 0.0f, 0.71f }, pi);
//
//
//
//std::array<Quaternion, 5> interpolate = {};
//
//std::array<float, interpolate.size()> t = { 0.0f, 0.3f, 0.5f, 0.7f, 1.0f };
//
//
//
//for (int i = 0; i < interpolate.size(); ++i)
//
//{
//
//	interpolate[i] = Slerp(rotation0, rotation1, t[i]);
//
//}

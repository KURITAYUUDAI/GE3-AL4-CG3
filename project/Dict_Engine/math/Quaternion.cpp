#include "Quaternion.h"

// Quaternionの積
Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs)
{
	Vector3 cross = Cross(Vector3{ lhs.x, lhs.y, lhs.z }, Vector3{ rhs.x, rhs.y, rhs.z });
	return
	{
		cross.x + rhs.w * lhs.x + lhs.w * rhs.x,
		cross.y + rhs.w * lhs.y + lhs.w * rhs.y,
		cross.z + rhs.w * lhs.z + lhs.w * rhs.z,
		lhs.w * rhs.w - Dot(Vector3{ lhs.x, lhs.y, lhs.z }, Vector3{ rhs.x, rhs.y, rhs.z }),
	};
}

// 単位Quaternionを返す
Quaternion IdentityQuaternion()
{
	return { 0.0f, 0.0f, 0.0f, 1.0f };
}

// 共役Quaternionを返す
Quaternion Conjugate(const Quaternion& quaternion)
{
	return
	{
		-quaternion.x,
		-quaternion.y,
		-quaternion.z,
		quaternion.w
	};
}

// Quaternionのnormを返す
float Norm(const Quaternion& quaternion)
{
	return std::sqrtf(
		std::powf(quaternion.x, 2) +
		std::powf(quaternion.y, 2) +
		std::powf(quaternion.z, 2) +
		std::powf(quaternion.w, 2));
}

// 正規化したQuaternionを返す
Quaternion Normalize(const Quaternion& quaternion)
{
	float norm = Norm(quaternion);
	return
	{
		quaternion.x / norm,
		quaternion.y / norm,
		quaternion.z / norm,
		quaternion.w / norm,
	};
}

// 逆Quaternionを返す
Quaternion Inverse(const Quaternion& quaternion)
{
	Quaternion conj = Conjugate(quaternion);
	float norm = Norm(quaternion);

	return
	{
		conj.x / std::powf(norm, 2),
		conj.y / std::powf(norm, 2),
		conj.z / std::powf(norm, 2),
		conj.w / std::powf(norm, 2),
	};
}

// 任意軸回転を表すQuaternionの生成
Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle)
{
	return
	{
		axis.x * std::sinf(angle / 2.0f),
		axis.y * std::sinf(angle / 2.0f),
		axis.z * std::sinf(angle / 2.0f),
		std::cosf(angle / 2.0f)
	};
}

// ベクトルをQuaternionで回転させた結果のベクトルを求める
Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion)
{
	Quaternion v = { vector.x, vector.y, vector.z, 0.0f };
	Quaternion qConj = Conjugate(quaternion);
	Quaternion rotatedV = Multiply(Multiply(quaternion, v), qConj);
	return { rotatedV.x, rotatedV.y, rotatedV.z };
}

// Quaternionから回転行列を求める
Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion)
{
	float xx = quaternion.x * quaternion.x;
	float yy = quaternion.y * quaternion.y;
	float zz = quaternion.z * quaternion.z;
	float ww = quaternion.w * quaternion.w;

	return
	{
		ww + xx - yy - zz,
		2.0f * (quaternion.x * quaternion.y + quaternion.w * quaternion.z),
		2.0f * (quaternion.x * quaternion.z - quaternion.w * quaternion.y),
		0.0f,

		2.0f * (quaternion.x * quaternion.y - quaternion.w * quaternion.z),
		ww - xx + yy - zz,
		2.0f * (quaternion.y * quaternion.z + quaternion.w * quaternion.x),
		0.0f,

		2.0f * (quaternion.x * quaternion.z + quaternion.w * quaternion.y),
		2.0f * (quaternion.y * quaternion.z - quaternion.w * quaternion.x),
		ww - xx - yy + zz,
		0.0f,

		0.0f, 0.0f, 0.0f, 1.0f
	};
}

float Dot(const Quaternion& q1, const Quaternion& q2)
{
	return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
}

// 球面線形補間　使う前に必ず正規化を掛けること
Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t)
{
	Quaternion q = q0;
	float dot = Dot(q0, q1);
	if (dot < 0.0f)
	{
		q = { -q0.x, -q0.y, -q0.z, -q0.w };
		dot = -dot;
	}

	float theta = std::acosf(dot);

	float scale0 = std::sinf((1.0f - t) * theta) / std::sinf(theta);
	float scale1 = std::sinf(t * theta) / std::sinf(theta);

	return scale0 * q + scale1 * q1;
}

Quaternion MakeFromEuler(const Vector3& euler) 
{
	// 各軸の半角のサイン・コサインを計算
	float cx = std::cosf(euler.x * 0.5f);
	float sx = std::sinf(euler.x * 0.5f);
	float cy = std::cosf(euler.y * 0.5f);
	float sy = std::sinf(euler.y * 0.5f);
	float cz = std::cosf(euler.z * 0.5f);
	float sz = std::sinf(euler.z * 0.5f);

	//// XYZ順で回転を合成したQuaternionの公式
	//return {
	//	sx * cy * cz - cx * sy * sz,
	//	cx * sy * cz + sx * cy * sz,
	//	cx * cy * sz - sx * sy * cz,
	//	cx * cy * cz + sx * sy * sz
	//};

	// YXZ回転
	return Quaternion(
		cx * sy * sz + sx * cy * cz,
		-sx * cy * sz + cx * sy * cz,
		cx * cy * sz - sx * sy * cz,
		sx * sy * sz + cx * cy * cz
	);
}

// YXZ回転
Vector3 MakeToEuler(const Quaternion q)
{
	auto sx = -(2.0f * q.y * q.z - 2.0f * q.x * q.w);
	auto unlocked = std::abs(sx) < 0.99999f;

	return Vector3(
		std::asin(sx),

		unlocked ? std::atan2(2.0f * q.x * q.z + 2.0f * q.y * q.w, 2.0f * q.w * q.w + 2.0f * q.z * q.z - 1)
			: std::atan2(-(2.0f * q.x * q.z - 2.0f * q.y * q.w), 2.0f * q.w * q.w + 2.0f * q.x * q.x - 1),
	  
		unlocked ? std::atan2(2.0f * q.x * q.y + 2.0f * q.z * q.w, 2.0f * q.w * q.w + 2.0f * q.y * q.y - 1) : 0.0f
	);
}

// YXZ回転
Quaternion MakeFromMatrix(const Matrix4x4& m) 
{
	float px = m.m[0][0] - m.m[1][1] - m.m[2][2] + 1.0f;
	float py = -m.m[0][0] + m.m[1][1] - m.m[2][2] + 1.0f;
	float pz = -m.m[0][0] - m.m[1][1] + m.m[2][2] + 1.0f;
	float pw = m.m[0][0] + m.m[1][1] + m.m[2][2] + 1.0f;

	int selected = 0;
	float max = px;
	if (max < py) { selected = 1; max = py; }
	if (max < pz) { selected = 2; max = pz; }
	if (max < pw) { selected = 3; max = pw; }

	Quaternion q;
	if (selected == 0) 
	{
		float x = std::sqrt(px) * 0.5f;
		float d = 1.0f / (4.0f * x);
		q.x = x;
		q.y = (m.m[1][0] + m.m[0][1]) * d;
		q.z = (m.m[0][2] + m.m[2][0]) * d;
		q.w = (m.m[2][1] - m.m[1][2]) * d;
	} 
	else if (selected == 1) 
	{
		float y = std::sqrt(py) * 0.5f;
		float d = 1.0f / (4.0f * y);
		q.x = (m.m[1][0] + m.m[0][1]) * d;
		q.y = y;
		q.z = (m.m[2][1] + m.m[1][2]) * d;
		q.w = (m.m[0][2] - m.m[2][0]) * d;
	} 
	else if (selected == 2) 
	{
		float z = std::sqrt(pz) * 0.5f;
		float d = 1.0f / (4.0f * z);
		q.x = (m.m[0][2] + m.m[2][0]) * d;
		q.y = (m.m[2][1] + m.m[1][2]) * d;
		q.z = z;
		q.w = (m.m[1][0] - m.m[0][1]) * d;
	} 
	else if (selected == 3) 
	{
		float w = std::sqrt(pw) * 0.5f;
		float d = 1.0f / (4.0f * w);
		q.x = (m.m[2][1] - m.m[1][2]) * d;
		q.y = (m.m[0][2] - m.m[2][0]) * d;
		q.z = (m.m[1][0] - m.m[0][1]) * d;
		q.w = w;
	} 
	else 
	{
		throw "conversion of rotation matrix to quaternion failed.";
	}

	return q;
}

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Quaternion& rotate, const Vector3& translate)
{
	// Quaternion → 回転行列（正規化済み前提）
	float xx = rotate.x * rotate.x;
	float yy = rotate.y * rotate.y;
	float zz = rotate.z * rotate.z;
	float xy = rotate.x * rotate.y;
	float xz = rotate.x * rotate.z;
	float yz = rotate.y * rotate.z;
	float wx = rotate.w * rotate.x;
	float wy = rotate.w * rotate.y;
	float wz = rotate.w * rotate.z;

	Matrix4x4 m;

	m.m[0][0] = scale.x * (1.0f - 2.0f * (yy + zz));
	m.m[0][1] = scale.x * (2.0f * (xy + wz));
	m.m[0][2] = scale.x * (2.0f * (xz - wy));
	m.m[0][3] = 0.0f;

	m.m[1][0] = scale.y * (2.0f * (xy - wz));
	m.m[1][1] = scale.y * (1.0f - 2.0f * (xx + zz));
	m.m[1][2] = scale.y * (2.0f * (yz + wx));
	m.m[1][3] = 0.0f;

	m.m[2][0] = scale.z * (2.0f * (xz + wy));
	m.m[2][1] = scale.z * (2.0f * (yz - wx));
	m.m[2][2] = scale.z * (1.0f - 2.0f * (xx + yy));
	m.m[2][3] = 0.0f;

	m.m[3][0] = translate.x;
	m.m[3][1] = translate.y;
	m.m[3][2] = translate.z;
	m.m[3][3] = 1.0f;

	return m;
}




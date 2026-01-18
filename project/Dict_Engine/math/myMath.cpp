#include "myMath.h"

// 00_01
// 加算
Vector3 Add(const Vector3& v1, const Vector3& v2)
{
	return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

// 減算
Vector3 Subtract(const Vector3& v1, const Vector3& v2)
{
	return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

// スカラー倍
Vector3 Multiply(float scalar, const Vector3& v)
{
	return { scalar * v.x, scalar * v.y, scalar * v.z };
}

// 内積
float Dot(const Vector3& v1, const Vector3& v2)
{
	return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

// 長さ(ノルム)
float Length(const Vector3& v)
{
	return sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

// 正規化
Vector3 Normalize(const Vector3& v)
{
	float length = Length(v);

	return { v.x / length, v.y / length, v.z / length };
}

// 00_02
// 1. 行列の加法
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2)
{
	Matrix4x4 m3;
	m3 =
	{
		m1.m[0][0] + m2.m[0][0], m1.m[0][1] + m2.m[0][1], m1.m[0][2] + m2.m[0][2], m1.m[0][3] + m2.m[0][3],
		m1.m[1][0] + m2.m[1][0], m1.m[1][1] + m2.m[1][1], m1.m[1][2] + m2.m[1][2], m1.m[1][3] + m2.m[1][3],
		m1.m[2][0] + m2.m[2][0], m1.m[2][1] + m2.m[2][1], m1.m[2][2] + m2.m[2][2], m1.m[2][3] + m2.m[2][3],
		m1.m[3][0] + m2.m[3][0], m1.m[3][1] + m2.m[3][1], m1.m[3][2] + m2.m[3][2], m1.m[3][3] + m2.m[3][3],
	};
	return m3;
}

// 2. 行列の減法
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2)
{
	Matrix4x4 m3;
	m3 =
	{
		m1.m[0][0] - m2.m[0][0], m1.m[0][1] - m2.m[0][1], m1.m[0][2] - m2.m[0][2], m1.m[0][3] - m2.m[0][3],
		m1.m[1][0] - m2.m[1][0], m1.m[1][1] - m2.m[1][1], m1.m[1][2] - m2.m[1][2], m1.m[1][3] - m2.m[1][3],
		m1.m[2][0] - m2.m[2][0], m1.m[2][1] - m2.m[2][1], m1.m[2][2] - m2.m[2][2], m1.m[2][3] - m2.m[2][3],
		m1.m[3][0] - m2.m[3][0], m1.m[3][1] - m2.m[3][1], m1.m[3][2] - m2.m[3][2], m1.m[3][3] - m2.m[3][3],
	};
	return m3;
}

// 3. 行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2)
{
	Matrix4x4 m3;
	float factor1[4], factor2[4] = { 0.0f };
	for (int row = 0; row < 4; ++row)
	{
		factor1[0] = m1.m[row][0];
		factor1[1] = m1.m[row][1];
		factor1[2] = m1.m[row][2];
		factor1[3] = m1.m[row][3];
		for (int column = 0; column < 4; ++column)
		{
			factor2[0] = m2.m[0][column];
			factor2[1] = m2.m[1][column];
			factor2[2] = m2.m[2][column];
			factor2[3] = m2.m[3][column];
			m3.m[row][column]
				= factor1[0] * factor2[0] + factor1[1] * factor2[1] + factor1[2] * factor2[2] + factor1[3] * factor2[3];
		}
	}
	return m3;
}

// 4. 逆行列
Matrix4x4 Inverse(const Matrix4x4& a)
{
	Matrix4x4 inverseM;
	inverseM.m[0][0]
		= (a.m[1][1] * a.m[2][2] * a.m[3][3]) + (a.m[1][2] * a.m[2][3] * a.m[3][1]) + (a.m[1][3] * a.m[2][1] * a.m[3][2])
		- (a.m[1][3] * a.m[2][2] * a.m[3][1]) - (a.m[1][2] * a.m[2][1] * a.m[3][3]) - (a.m[1][1] * a.m[2][3] * a.m[3][2]);
	inverseM.m[0][1]
		= -(a.m[0][1] * a.m[2][2] * a.m[3][3]) - (a.m[0][2] * a.m[2][3] * a.m[3][1]) - (a.m[0][3] * a.m[2][1] * a.m[3][2])
		+ (a.m[0][3] * a.m[2][2] * a.m[3][1]) + (a.m[0][2] * a.m[2][1] * a.m[3][3]) + (a.m[0][1] * a.m[2][3] * a.m[3][2]);
	inverseM.m[0][2]
		= (a.m[0][1] * a.m[1][2] * a.m[3][3]) + (a.m[0][2] * a.m[1][3] * a.m[3][1]) + (a.m[0][3] * a.m[1][1] * a.m[3][2])
		- (a.m[0][3] * a.m[1][2] * a.m[3][1]) - (a.m[0][2] * a.m[1][1] * a.m[3][3]) - (a.m[0][1] * a.m[1][3] * a.m[3][2]);
	inverseM.m[0][3]
		= -(a.m[0][1] * a.m[1][2] * a.m[2][3]) - (a.m[0][2] * a.m[1][3] * a.m[2][1]) - (a.m[0][3] * a.m[1][1] * a.m[2][2])
		+ (a.m[0][3] * a.m[1][2] * a.m[2][1]) + (a.m[0][2] * a.m[1][1] * a.m[2][3]) + (a.m[0][1] * a.m[1][3] * a.m[2][2]);

	inverseM.m[1][0]
		= -(a.m[1][0] * a.m[2][2] * a.m[3][3]) - (a.m[1][2] * a.m[2][3] * a.m[3][0]) - (a.m[1][3] * a.m[2][0] * a.m[3][2])
		+ (a.m[1][3] * a.m[2][2] * a.m[3][0]) + (a.m[1][2] * a.m[2][0] * a.m[3][3]) + (a.m[1][0] * a.m[2][3] * a.m[3][2]);
	inverseM.m[1][1]
		= (a.m[0][0] * a.m[2][2] * a.m[3][3]) + (a.m[0][2] * a.m[2][3] * a.m[3][0]) + (a.m[0][3] * a.m[2][0] * a.m[3][2])
		- (a.m[0][3] * a.m[2][2] * a.m[3][0]) - (a.m[0][2] * a.m[2][0] * a.m[3][3]) - (a.m[0][0] * a.m[2][3] * a.m[3][2]);
	inverseM.m[1][2]
		= -(a.m[0][0] * a.m[1][2] * a.m[3][3]) - (a.m[0][2] * a.m[1][3] * a.m[3][0]) - (a.m[0][3] * a.m[1][0] * a.m[3][2])
		+ (a.m[0][3] * a.m[1][2] * a.m[3][0]) + (a.m[0][2] * a.m[1][0] * a.m[3][3]) + (a.m[0][0] * a.m[1][3] * a.m[3][2]);
	inverseM.m[1][3]
		= (a.m[0][0] * a.m[1][2] * a.m[2][3]) + (a.m[0][2] * a.m[1][3] * a.m[2][0]) + (a.m[0][3] * a.m[1][0] * a.m[2][2])
		- (a.m[0][3] * a.m[1][2] * a.m[2][0]) - (a.m[0][2] * a.m[1][0] * a.m[2][3]) - (a.m[0][0] * a.m[1][3] * a.m[2][2]);

	inverseM.m[2][0]
		= (a.m[1][0] * a.m[2][1] * a.m[3][3]) + (a.m[1][1] * a.m[2][3] * a.m[3][0]) + (a.m[1][3] * a.m[2][0] * a.m[3][1])
		- (a.m[1][3] * a.m[2][1] * a.m[3][0]) - (a.m[1][1] * a.m[2][0] * a.m[3][3]) - (a.m[1][0] * a.m[2][3] * a.m[3][1]);
	inverseM.m[2][1]
		= -(a.m[0][0] * a.m[2][1] * a.m[3][3]) - (a.m[0][1] * a.m[2][3] * a.m[3][0]) - (a.m[0][3] * a.m[2][0] * a.m[3][1])
		+ (a.m[0][3] * a.m[2][1] * a.m[3][0]) + (a.m[0][1] * a.m[2][0] * a.m[3][3]) + (a.m[0][0] * a.m[2][3] * a.m[3][1]);
	inverseM.m[2][2]
		= (a.m[0][0] * a.m[1][1] * a.m[3][3]) + (a.m[0][1] * a.m[1][3] * a.m[3][0]) + (a.m[0][3] * a.m[1][0] * a.m[3][1])
		- (a.m[0][3] * a.m[1][1] * a.m[3][0]) - (a.m[0][1] * a.m[1][0] * a.m[3][3]) - (a.m[0][0] * a.m[1][3] * a.m[3][1]);
	inverseM.m[2][3]
		= -(a.m[0][0] * a.m[1][1] * a.m[2][3]) - (a.m[0][1] * a.m[1][3] * a.m[2][0]) - (a.m[0][3] * a.m[1][0] * a.m[2][1])
		+ (a.m[0][3] * a.m[1][1] * a.m[2][0]) + (a.m[0][1] * a.m[1][0] * a.m[2][3]) + (a.m[0][0] * a.m[1][3] * a.m[2][1]);

	inverseM.m[3][0]
		= -(a.m[1][0] * a.m[2][1] * a.m[3][2]) - (a.m[1][1] * a.m[2][2] * a.m[3][0]) - (a.m[1][2] * a.m[2][0] * a.m[3][1])
		+ (a.m[1][2] * a.m[2][1] * a.m[3][0]) + (a.m[1][1] * a.m[2][0] * a.m[3][2]) + (a.m[1][0] * a.m[2][2] * a.m[3][1]);
	inverseM.m[3][1]
		= (a.m[0][0] * a.m[2][1] * a.m[3][2]) + (a.m[0][1] * a.m[2][2] * a.m[3][0]) + (a.m[0][2] * a.m[2][0] * a.m[3][1])
		- (a.m[0][2] * a.m[2][1] * a.m[3][0]) - (a.m[0][1] * a.m[2][0] * a.m[3][2]) - (a.m[0][0] * a.m[2][2] * a.m[3][1]);
	inverseM.m[3][2]
		= -(a.m[0][0] * a.m[1][1] * a.m[3][2]) - (a.m[0][1] * a.m[1][2] * a.m[3][0]) - (a.m[0][2] * a.m[1][0] * a.m[3][1])
		+ (a.m[0][2] * a.m[1][1] * a.m[3][0]) + (a.m[0][1] * a.m[1][0] * a.m[3][2]) + (a.m[0][0] * a.m[1][2] * a.m[3][1]);
	inverseM.m[3][3]
		= (a.m[0][0] * a.m[1][1] * a.m[2][2]) + (a.m[0][1] * a.m[1][2] * a.m[2][0]) + (a.m[0][2] * a.m[1][0] * a.m[2][1])
		- (a.m[0][2] * a.m[1][1] * a.m[2][0]) - (a.m[0][1] * a.m[1][0] * a.m[2][2]) - (a.m[0][0] * a.m[1][2] * a.m[2][1]);

	float A =
		a.m[0][0] * a.m[1][1] * a.m[2][2] * a.m[3][3] + a.m[0][0] * a.m[1][2] * a.m[2][3] * a.m[3][1] + a.m[0][0] * a.m[1][3] * a.m[2][1] * a.m[3][2]
		- a.m[0][0] * a.m[1][3] * a.m[2][2] * a.m[3][1] - a.m[0][0] * a.m[1][2] * a.m[2][1] * a.m[3][3] - a.m[0][0] * a.m[1][1] * a.m[2][3] * a.m[3][2]
		- a.m[0][1] * a.m[1][0] * a.m[2][2] * a.m[3][3] - a.m[0][2] * a.m[1][0] * a.m[2][3] * a.m[3][1] - a.m[0][3] * a.m[1][0] * a.m[2][1] * a.m[3][2]
		+ a.m[0][3] * a.m[1][0] * a.m[2][2] * a.m[3][1] + a.m[0][2] * a.m[1][0] * a.m[2][1] * a.m[3][3] + a.m[0][1] * a.m[1][0] * a.m[2][3] * a.m[3][2]
		+ a.m[0][1] * a.m[1][2] * a.m[2][0] * a.m[3][3] + a.m[0][2] * a.m[1][3] * a.m[2][0] * a.m[3][1] + a.m[0][3] * a.m[1][1] * a.m[2][0] * a.m[3][2]
		- a.m[0][3] * a.m[1][2] * a.m[2][0] * a.m[3][1] - a.m[0][2] * a.m[1][1] * a.m[2][0] * a.m[3][3] - a.m[0][1] * a.m[1][3] * a.m[2][0] * a.m[3][2]
		- a.m[0][1] * a.m[1][2] * a.m[2][3] * a.m[3][0] - a.m[0][2] * a.m[1][3] * a.m[2][1] * a.m[3][0] - a.m[0][3] * a.m[1][1] * a.m[2][2] * a.m[3][0]
		+ a.m[0][3] * a.m[1][2] * a.m[2][1] * a.m[3][0] + a.m[0][2] * a.m[1][1] * a.m[2][3] * a.m[3][0] + a.m[0][1] * a.m[1][3] * a.m[2][2] * a.m[3][0];

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			inverseM.m[i][j] *= 1 / A;
		}
	}

	return inverseM;
}

// 3×3 の小行列式を計算するヘルパー
float det3(const Matrix4x4& a,
	int r0, int c0,
	int r1, int c1,
	int r2, int c2)
{
	return
		a.m[r0][c0] * (a.m[r1][c1] * a.m[r2][c2] - a.m[r1][c2] * a.m[r2][c1])
		- a.m[r0][c1] * (a.m[r1][c0] * a.m[r2][c2] - a.m[r1][c2] * a.m[r2][c0])
		+ a.m[r0][c2] * (a.m[r1][c0] * a.m[r2][c1] - a.m[r1][c1] * a.m[r2][c0]);
}

// 逆行列を返す関数
Matrix4x4 Inverse4(const Matrix4x4& a)
{
	// 行列式を 1 行目展開で計算
	float A =
		a.m[0][0] * det3(a, 1, 1, 2, 2, 3, 3)
		- a.m[0][1] * det3(a, 1, 0, 2, 2, 3, 3)
		+ a.m[0][2] * det3(a, 1, 0, 2, 1, 3, 3)
		- a.m[0][3] * det3(a, 1, 0, 2, 1, 3, 2);

	// 余因子行列 C を求める
	Matrix4x4 C;
	C.m[0][0] = det3(a, 1, 1, 2, 2, 3, 3);
	C.m[0][1] = -det3(a, 0, 1, 2, 2, 3, 3);
	C.m[0][2] = det3(a, 0, 1, 1, 2, 3, 3);
	C.m[0][3] = -det3(a, 0, 1, 1, 2, 2, 3);

	C.m[1][0] = -det3(a, 1, 0, 2, 2, 3, 3);
	C.m[1][1] = det3(a, 0, 0, 2, 2, 3, 3);
	C.m[1][2] = -det3(a, 0, 0, 1, 2, 3, 3);
	C.m[1][3] = det3(a, 0, 0, 1, 2, 2, 3);

	C.m[2][0] = det3(a, 1, 0, 2, 1, 3, 3);
	C.m[2][1] = -det3(a, 0, 0, 2, 1, 3, 3);
	C.m[2][2] = det3(a, 0, 0, 1, 1, 3, 3);
	C.m[2][3] = -det3(a, 0, 0, 1, 1, 2, 3);

	C.m[3][0] = -det3(a, 1, 0, 2, 1, 3, 2);
	C.m[3][1] = det3(a, 0, 0, 2, 1, 3, 2);
	C.m[3][2] = -det3(a, 0, 0, 1, 1, 3, 2);
	C.m[3][3] = det3(a, 0, 0, 1, 1, 2, 2);

	// 余因子行列の転置（＝随伴行列）を行列式で割る
	Matrix4x4 inv;
	float invA = 1.0f / A;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			inv.m[i][j] = C.m[j][i] * invA;
		}
	}
	return inv;
}

// 5. 転置行列
Matrix4x4 Transpose(const Matrix4x4& m)
{
	Matrix4x4 m1 = { 0.0f };

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m1.m[i][j] = m.m[j][i];
		}
	}

	return m1;
}

// 6. 単位行列の作成
Matrix4x4 MakeIdentity4x4()
{
	return { 1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f };
}
// 00_03
// 1. 平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate)
{
	return
	{
		1.0f,		 0.0f,		  0.0f,		   0.0f,
		0.0f,		 1.0f,		  0.0f,		   0.0f,
		0.0f,		 0.0f,		  1.0f,		   0.0f,
		translate.x, translate.y, translate.z, 1.0f
	};
}

// 2. 拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale)
{
	return
	{
		scale.x,	0.0f,		0.0f,		0.0f,
		0.0f,		scale.y,	0.0f,		0.0f,
		0.0f,		0.0f,		scale.z,	0.0f,
		0.0f,		0.0f,		0.0f,		1.0f
	};
}

Vector3 TransformPosition(const Vector3& vector, const Matrix4x4& matrix)
{
	Vector3 result;
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;
	return result;
}

// 00_04
// 1. X軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian)
{
	return
	{
		 1.0f,	 0.0f,			   0.0f,			 0.0f,
		 0.0f,   std::cos(radian), std::sin(radian), 0.0f,
		 0.0f,	-std::sin(radian), std::cos(radian), 0.0f,
		 0.0f,	 0.0f,			   0.0f,			 1.0f
	};
}

// 2. Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian)
{
	return
	{
		 std::cos(radian), 0.0f, -std::sin(radian), 0.0f,
		 0.0f,			   1.0f,  0.0f,				0.0f,
		 std::sin(radian), 0.0f,  std::cos(radian), 0.0f,
		 0.0f,			   0.0f,  0.0f,				1.0f
	};
}

// 3. Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian)
{
	return
	{
		 std::cos(radian), std::sin(radian), 0.0f, 0.0f,
		-std::sin(radian), std::cos(radian), 0.0f, 0.0f,
		 0.0f,			   0.0f,			 1.0f, 0.0f,
		 0.0f,			   0.0f,			 0.0f, 1.0f
	};
}

// 00_05
// 3次元アフィン変換行列（Y → X → Z）
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate)
{
	Matrix4x4 rotateMatrix = Multiply(MakeRotateZMatrix(rotate.z),
		Multiply(MakeRotateXMatrix(rotate.x), MakeRotateYMatrix(rotate.y)));

	return Multiply(Multiply(MakeScaleMatrix(scale), rotateMatrix), MakeTranslateMatrix(translate));
}

// 3次元アフィン変換行列(GPT製)（Y → X → Z）
Matrix4x4 MakeAffineMatrixB(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	float cx = cosf(rotate.x), sx = sinf(rotate.x);
	float cy = cosf(rotate.y), sy = sinf(rotate.y);
	float cz = cosf(rotate.z), sz = sinf(rotate.z);

	Matrix4x4 m;

	m.m[0][0] = scale.x * (cz * cy + sz * sx * sy);
	m.m[0][1] = scale.x * (sz * cx);
	m.m[0][2] = scale.x * (cz * -sy + sz * sx * cy);
	m.m[0][3] = 0.0f;

	m.m[1][0] = scale.y * (-sz * cy + cz * sx * sy);
	m.m[1][1] = scale.y * (cz * cx);
	m.m[1][2] = scale.y * (-sz * -sy + cz * sx * cy);
	m.m[1][3] = 0.0f;

	m.m[2][0] = scale.z * (cx * sy);
	m.m[2][1] = scale.z * (-sx);
	m.m[2][2] = scale.z * (cx * cy);
	m.m[2][3] = 0.0f;

	m.m[3][0] = translate.x;
	m.m[3][1] = translate.y;
	m.m[3][2] = translate.z;
	m.m[3][3] = 1.0f;

	return m;
}

// cot（コタンジェント）関数が無いので作る
float cot(float theta)
{
	return 1.0f / std::tan(theta);
}

// 1. 透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip)
{
	return
	{
		(1.0f / aspectRatio) * cot(fovY / 2.0f), 0.0f, 0.0f, 0.0f,
		0.0f, cot(fovY / 2.0f), 0.0f, 0.0f,
		0.0f, 0.0f, farClip / (farClip - nearClip), 1.0f,
		0.0f, 0.0f, (-nearClip * farClip) / (farClip - nearClip), 0.0f,
	};
}

// 2. 正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip)
{
	return
	{
		2.0f / (right - left), 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f / (farClip - nearClip), 0.0f,
		(left + right) / (left - right), (top + bottom) / (bottom - top), nearClip / (nearClip - farClip), 1.0f
	};
}

// 3. ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth)
{
	return
	{
		width / 2.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -height / 2.0f, 0.0f, 0.0f,
		0.0f, 0.0f, maxDepth - minDepth, 0.0f,
		left + (width / 2.0f), top + (height / 2.0f), minDepth, 1.0f
	};
}

// 01_01
// クロス積関数
Vector3 Cross(const Vector3& a, const Vector3& b)
{
	return
	{
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	};
}

void MakeRegularTriangle(const Vector3& center, float sideLength, Vector3 vertices[3])
{
	// 正三角形の高さを計算
	float height = std::sqrt(3.0f) / 2.0f * sideLength;

	// 頂点の計算
	vertices[0] = { center.x, center.y + 2.0f / 3.0f * height, center.z };                     // 上の頂点
	vertices[1] = { center.x - sideLength / 2.0f, center.y - 1.0f / 3.0f * height, center.z }; // 左下の頂点
	vertices[2] = { center.x + sideLength / 2.0f, center.y - 1.0f / 3.0f * height, center.z }; // 右下の頂点
}

void RotateAngleCorrection(float& rotate)
{
	if (rotate > float(std::numbers::pi_v<float>))
	{
		rotate = rotate - 2.0f * float(std::numbers::pi_v<float>);
	}
	if (rotate < -float(std::numbers::pi_v<float>))
	{
		rotate = rotate + 2.0f * float(std::numbers::pi_v<float>);
	}
}

Vector3 Project(const Vector3& v1, const Vector3& v2)
{
	// a・b / ||b||²
	float t = Dot(v1, v2) / powf(Length(v2), 2);

	return
	{
		t * v2.x,
		t * v2.y,
		t * v2.z,
	};
}

Vector3 ClosestPoint(const Vector3& point, const Segment& segment)
{

	Vector3 projBA = Project(Subtract(point, segment.origin), segment.diff);

	return Add(segment.origin, projBA);
}

bool IsCollision(const Sphere& sphere1, const Sphere& sphere2)
{
	// 2つの球の中心点間の距離を求める
	float distance = Length(Subtract(sphere2.center, sphere1.center));
	// 半径の合計よりも短ければ衝突
	if (distance <= sphere1.radius + sphere2.radius)
	{
		return true;
	} else
	{
		return false;
	}
}

bool IsCollision(const Sphere& sphere, const Plane& plane)
{
	// 平面と球の中心点間の距離を求める
	float distance = Dot(plane.normal, sphere.center) - plane.distance;
	// 半径よりも短ければ衝突
	if (std::abs(distance) <= sphere.radius)
	{
		return true;
	} else
	{
		return false;
	}
}

bool IsCollision(const Plane& plane, const Line& line)
{
	// 法線と線の内積を求める
	float dot = Dot(plane.normal, line.diff);
	// 内積が0であれば平行
	if (dot == 0.0f)
	{
		return false;
	}

	// 平行でなければ直線は衝突する
	return true;
}

bool IsCollision(const Plane& plane, const Ray& ray)
{
	// 法線と線の内積を求める
	float dot = Dot(plane.normal, ray.diff);
	// 内積が0であれば平行
	if (dot == 0.0f)
	{
		return false;
	}

	// tを求める
	float t = (plane.distance - Dot(ray.origin, plane.normal)) / dot;

	// tが0未満であれば半直線は衝突しない
	if (t < 0.0f)
	{
		return false;
	}

	// tが0以上であれば半直線は衝突する
	return true;
}

// 線分と平面の衝突判定を求める関数
bool IsCollision(const Plane& plane, const Segment& segment)
{
	// 法線と線の内積を求める
	float dot = Dot(plane.normal, segment.diff);

	// 内積が0であれば平行
	if (dot == 0.0f)
	{
		return false;
	}

	// tを求める
	float t = (plane.distance - Dot(segment.origin, plane.normal)) / dot;

	// tが0未満または1より大きければ線分は衝突しない
	if (t < 0.0f || t > 1.0f)
	{
		return false;
	}

	// tが0以上1以下であれば線分は衝突する
	return true;
}

bool IsCollision(const Triangle& triangle, const Segment& segment)
{
	// 三角形の頂点から平面を求める
	Plane plane;
	plane.normal =
		Normalize(Cross(Subtract(triangle.vertices[1], triangle.vertices[0]),
			Subtract(triangle.vertices[2], triangle.vertices[1])));
	plane.distance = Dot(triangle.vertices[0], plane.normal);

	// 線分と平面の衝突判定を求める

	// 法線と線の内積を求める
	float dot = Dot(plane.normal, segment.diff);

	// 内積が0であれば平行
	if (dot == 0.0f)
	{
		return false;
	}

	// tを求める
	float t = (plane.distance - Dot(segment.origin, plane.normal)) / dot;

	// tが0未満または1より大きければ線分は衝突しない
	if (t < 0.0f || t > 1.0f)
	{
		return false;
	}

	// 衝突している場合、三角形の内側かどうかを判定する
	Vector3 p = Add(segment.origin, Multiply(t, segment.diff)); // 衝突点

	// 各辺を結んだベクトルと、頂点と衝突点を結んだベクトルのクロス積を求める
	Vector3 cross01 = Cross(Subtract(triangle.vertices[1], triangle.vertices[0]), Subtract(p, triangle.vertices[1]));
	Vector3 cross12 = Cross(Subtract(triangle.vertices[2], triangle.vertices[1]), Subtract(p, triangle.vertices[2]));
	Vector3 cross20 = Cross(Subtract(triangle.vertices[0], triangle.vertices[2]), Subtract(p, triangle.vertices[0]));

	// すべての小三角形のクロス積と法線が同じ方向を向いていたら衝突
	if (Dot(cross01, plane.normal) >= 0.0f
		&& Dot(cross12, plane.normal) >= 0.0f
		&& Dot(cross20, plane.normal) >= 0.0f)
	{
		return true;
	}

	return false;
}

bool IsCollision(const AABB& aabb, const Vector3& point)
{
	if ((aabb.min.x <= point.x && aabb.max.x >= point.x) && 
		(aabb.min.y <= point.y && aabb.max.y >= point.y) && 
		(aabb.min.z <= point.z && aabb.max.z >= point.z)) 
	{
		return true; // 衝突している
	}

	return false; // 衝突していない
}

bool IsCollision(const AABB& aabb1, const AABB& aabb2)
{
	if ((aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) && (aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) && (aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z)) {
		return true; // 衝突している
	}

	return false; // 衝突していない
}

bool IsCollision(const AABB& aabb, const Sphere& sphere)
{
	float loX = std::min(aabb.min.x, aabb.max.x);
	float hiX = std::max(aabb.min.x, aabb.max.x);
	float loY = std::min(aabb.min.y, aabb.max.y);
	float hiY = std::max(aabb.min.y, aabb.max.y);
	float loZ = std::min(aabb.min.z, aabb.max.z);
	float hiZ = std::max(aabb.min.z, aabb.max.z);

	Vector3 closestPoint{ std::clamp(sphere.center.x, loX, hiX), std::clamp(sphere.center.y, loY, hiY), std::clamp(sphere.center.z, loZ, hiZ) };
	// 最近接点と球の中心との距離を求める
	float distance = Length(Subtract(closestPoint, sphere.center));
	// 半径よりも短ければ衝突
	if (distance <= sphere.radius)
	{
		return true; // 衝突している
	}

	return false; // 衝突していない
}

bool IsCollision(const AABB& aabb, const Segment& segment)
{
	float txmin = (aabb.min.x - segment.origin.x) / segment.diff.x;
	float txmax = (aabb.max.x - segment.origin.x) / segment.diff.x;
	float tymin = (aabb.min.y - segment.origin.y) / segment.diff.y;
	float tymax = (aabb.max.y - segment.origin.y) / segment.diff.y;
	float tzmin = (aabb.min.z - segment.origin.z) / segment.diff.z;
	float tzmax = (aabb.max.z - segment.origin.z) / segment.diff.z;

	float tNearX = std::min(txmin, txmax);
	float tFarX = std::max(txmin, txmax);
	float tNearY = std::min(tymin, tymax);
	float tFarY = std::max(tymin, tymax);
	float tNearZ = std::min(tzmin, tzmax);
	float tFarZ = std::max(tzmin, tzmax);

	// AABBとの衝突点（貫通点）のtが小さい方
	float tmin = std::max(std::max(tNearX, tNearY), tNearZ);
	// AABBとの衝突点（貫通点）のtが大きい方
	float tmax = std::min(std::min(tFarX, tFarY), tFarZ);
	if (tmin > tmax)
	{
		return false;
	}

	if (tmax < 0.0f || tmin > 1.0f)
	{
		return false;
	}

	return true;
}

bool IsCollision(const OBB& obb, const Sphere& sphere)
{
	Matrix4x4 obbWorldMatrix =
	{
		obb.orientations[0].x, obb.orientations[1].x, obb.orientations[2].x, 0.0f,
		obb.orientations[0].y, obb.orientations[1].y, obb.orientations[2].y, 0.0f,
		obb.orientations[0].z, obb.orientations[1].z, obb.orientations[2].z, 0.0f,
		obb.center.x,          obb.center.y,          obb.center.z,          1.0f
	};

	Vector3 centerInOBBLocalSpace = TransformPosition(sphere.center, Inverse(obbWorldMatrix));

	AABB aabbOBBLocal
	{
		.min = {-obb.size.x, -obb.size.y, -obb.size.z},
		.max = obb.size,
	};

	Sphere sphereOBBLocal{ .center = centerInOBBLocalSpace, .radius = sphere.radius };

	return IsCollision(aabbOBBLocal, sphereOBBLocal);
}

bool IsCollision(const OBB& obb, const Segment& segment)
{
	Matrix4x4 obbWorldMatrix =
	{
		obb.orientations[0].x, obb.orientations[1].x, obb.orientations[2].x, 0.0f,
		obb.orientations[0].y, obb.orientations[1].y, obb.orientations[2].y, 0.0f,
		obb.orientations[0].z, obb.orientations[1].z, obb.orientations[2].z, 0.0f,
		obb.center.x,          obb.center.y,          obb.center.z,          1.0f
	};

	Vector3 localOrigin = TransformPosition(segment.origin, Inverse(obbWorldMatrix));
	Vector3 localEnd = TransformPosition(Add(segment.origin, segment.diff), Inverse(obbWorldMatrix));

	AABB localAABB
	{
		.min = {-obb.size.x, -obb.size.y, -obb.size.z},
		.max = obb.size,
	};

	Segment localSegment{ .origin = localOrigin, .diff = Subtract(localEnd, localOrigin) };

	return IsCollision(localAABB, localSegment);
}

bool IsCollision(const OBB (&obb)[2])
{
	Vector3 axis[15]
	{
		obb[0].orientations[0], // OBB1のX軸
		obb[0].orientations[1], // OBB1のY軸
		obb[0].orientations[2], // OBB1のZ軸
		obb[1].orientations[0], // OBB2のX軸
		obb[1].orientations[1], // OBB2のY軸
		obb[1].orientations[2], // OBB2のZ軸
	};

	// OBB1とOBB2の各軸の外積を求める
	for (int32_t i = 0; i < 3; ++i)
	{
		for (int32_t j = 0; j < 3; ++j)
		{
			auto v = Cross(obb[0].orientations[i], obb[1].orientations[j]);
			if (Length(v) < 1e-6f)
			{
				continue; // 長さが0に近い場合は無視
			}
			axis[6 + i * 3 + j] = Normalize(v);
		}
	}

	Vector3 vertices[2][8];
	const size_t projSize = 8;

	// OBBの各頂点を求める
	for (int32_t index = 0; index < 2; index++)
	{
		Matrix4x4 obbWorldMatrix =
		{
			obb[index].orientations[0].x, obb[index].orientations[1].x, obb[index].orientations[2].x, 0.0f,
			obb[index].orientations[0].y, obb[index].orientations[1].y, obb[index].orientations[2].y, 0.0f,
			obb[index].orientations[0].z, obb[index].orientations[1].z, obb[index].orientations[2].z, 0.0f,
			obb[index].center.x,          obb[index].center.y,          obb[index].center.z,          1.0f
		};

		AABB aabbOBBLocal
		{
			.min = {-obb[index].size.x, -obb[index].size.y, -obb[index].size.z},
			.max = obb[index].size,
		};

		Vector3 verticesAABB[8];
		// AABBの8頂点を計算
		verticesAABB[0] = { aabbOBBLocal.min.x, aabbOBBLocal.min.y, aabbOBBLocal.min.z };
		verticesAABB[1] = { aabbOBBLocal.max.x, aabbOBBLocal.min.y, aabbOBBLocal.min.z };
		verticesAABB[2] = { aabbOBBLocal.max.x, aabbOBBLocal.max.y, aabbOBBLocal.min.z };
		verticesAABB[3] = { aabbOBBLocal.min.x, aabbOBBLocal.max.y, aabbOBBLocal.min.z };
		verticesAABB[4] = { aabbOBBLocal.min.x, aabbOBBLocal.min.y, aabbOBBLocal.max.z };
		verticesAABB[5] = { aabbOBBLocal.max.x, aabbOBBLocal.min.y, aabbOBBLocal.max.z };
		verticesAABB[6] = { aabbOBBLocal.max.x, aabbOBBLocal.max.y, aabbOBBLocal.max.z };
		verticesAABB[7] = { aabbOBBLocal.min.x, aabbOBBLocal.max.y, aabbOBBLocal.max.z };

		for (int32_t vertexNum = 0; vertexNum < 8; ++vertexNum)
		{
			// OBBのワールド座標系に変換
			vertices[index][vertexNum] = TransformPosition(verticesAABB[vertexNum], obbWorldMatrix);
		}
	}

	for (int32_t axisNum = 0; axisNum < 15; ++axisNum)
	{
		float L[2], min[2], max[2];

		std::vector<float> projection(projSize); // 投影結果（最大最小を求めやすくするためstd::vector）
		// 各OBBの頂点を投影する
		for (int32_t index = 0; index < 2; ++index)
		{
			for (int32_t vertexNum = 0; vertexNum < 8; ++vertexNum)
			{
				projection[vertexNum] = Dot(vertices[index][vertexNum], axis[axisNum]);
			}

			// 最小値と最大値を求める
			auto it_max = std::max_element(projection.begin(), projection.end());
			max[index] = (it_max != projection.end()) ? *it_max : 0.0f;
			auto it_min = std::min_element(projection.begin(), projection.end());
			min[index] = (it_min != projection.end()) ? *it_min : 0.0f;
			L[index] = max[index] - min[index];
		}

		float sumSpan = L[0] + L[1];                                              // 2つのOBBの投影の長さの和
		float longSpan = (std::max)(max[0], max[1]) - (std::min)(min[0], min[1]); // 2つのOBBの投影の長さの差)
		if (sumSpan <= longSpan)
		{
			// 投影の長さの和が投影の長さの差よりも大きい場合、衝突していない
			return false;
		}
	}

	// すべての軸で衝突している場合、衝突している
	return true;
}

float Larp(const float& n1, const float& n2, const float& t)
{
	return n2 * t + n1 * (1.0f - t);
}

float EaseOut(const float& n1, const float& n2, const float& t)
{
	return Larp(n1, n2, std::sin((t * std::numbers::pi_v<float>) / 2));
}

Matrix4x4 MakeLookRotation(const Vector3& forward, const Vector3& up)
{
	Vector3 xAxis = Normalize(forward);          // X+方向を正面に
	Vector3 zAxis = Normalize(Cross(xAxis, up)); // Z軸（奥行き）
	Vector3 yAxis = Cross(zAxis, xAxis);         // Y軸（上）

	Matrix4x4 result{};
	result.m[0][0] = xAxis.x;
	result.m[1][0] = xAxis.y;
	result.m[2][0] = xAxis.z;
	result.m[3][0] = 0.0f;

	result.m[0][1] = yAxis.x;
	result.m[1][1] = yAxis.y;
	result.m[2][1] = yAxis.z;
	result.m[3][1] = 0.0f;

	result.m[0][2] = zAxis.x;
	result.m[1][2] = zAxis.y;
	result.m[2][2] = zAxis.z;
	result.m[3][2] = 0.0f;

	result.m[0][3] = 0.0f;
	result.m[1][3] = 0.0f;
	result.m[2][3] = 0.0f;
	result.m[3][3] = 1.0f;

	return result;
}

Vector3 GetEulerFromMatrix(const Matrix4x4& m)
{
	Vector3 rotation;

	if (std::abs(m.m[2][0]) < 1.0f)
	{
		rotation.y = std::asin(m.m[2][0]);
		rotation.x = std::atan2(-m.m[2][1], m.m[2][2]);
		rotation.z = std::atan2(-m.m[1][0], m.m[0][0]);
	} else
	{
		// Gimbal lock対策
		rotation.y = (m.m[2][0] > 0.0f) ? +std::numbers::pi_v<float> / 2 : -std::numbers::pi_v<float> / 2;
		rotation.x = std::atan2(m.m[0][1], m.m[1][1]);
		rotation.z = 0.0f;
	}

	return rotation;
}

Matrix4x4 MakeLookRotationXAxis(const Vector3& xAxis, const Vector3& up)
{
	Vector3 x = Normalize(xAxis);
	Vector3 z = Normalize(Cross(up, x));
	Vector3 y = Cross(x, z);

	Matrix4x4 result{};
	result.m[0][0] = x.x;
	result.m[0][1] = x.y;
	result.m[0][2] = x.z;
	result.m[0][3] = 0.0f;
	result.m[1][0] = y.x;
	result.m[1][1] = y.y;
	result.m[1][2] = y.z;
	result.m[1][3] = 0.0f;
	result.m[2][0] = z.x;
	result.m[2][1] = z.y;
	result.m[2][2] = z.z;
	result.m[2][3] = 0.0f;
	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;
	return result;
}

Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m)
{
	Vector3 result
	{
		v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0],
		v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1],
		v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2],
	};

	return result;
}
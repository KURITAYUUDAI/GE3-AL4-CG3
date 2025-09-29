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

#pragma once
#include <cmath>
#include <stdlib.h>
#include <assert.h>
#include <numbers>

struct Vector2
{
	float x, y;
};

struct Vector3
{
	float x, y, z;
};

struct Vector4
{
	float x, y, z, w;
};

struct Matrix3x3
{
	float m[3][3];
};

struct Matrix4x4
{
	float m[4][4];
};

struct Transform
{
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

static const int kColumnWidth = 60;
static const int kRowHeight = 20;

static const float pi = std::numbers::pi_v<float>;

// 00_01
// 加算
Vector3 Add(const Vector3& v1, const Vector3& v2);

// 減算
Vector3 Subtract(const Vector3& v1, const Vector3& v2);

// スカラー倍
Vector3 Multiply(float scalar, const Vector3& v);

// 内積
float Dot(const Vector3& v1, const Vector3& v2);

// 長さ(ノルム)
float Length(const Vector3& v);

// 正規化
Vector3 Normalize(const Vector3& v);

// 00_02
// 1. 行列の加法
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2);

// 2. 行列の減法
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2);

// 3. 行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

// 4. 逆行列
Matrix4x4 Inverse(const Matrix4x4& a);

// 5. 転置行列
Matrix4x4 Transpose(const Matrix4x4& m);

// 6. 単位行列の作成
Matrix4x4 MakeIdentity4x4();

// 00_03
// 1. 平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

// 2. 拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale);

// 00_04
// 1. X軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian);

// 2. Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian);

// 3. Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian);

// 00_05
// 3次元アフィン変換行列（Y → X → Z）
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

// 3次元アフィン変換行列(GPT製)（Y → X → Z）
Matrix4x4 MakeAffineMatrixB(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

// cot（コタンジェント）関数が無いので作る
float cot(float theta);

// 1. 透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

// 2. 正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);

// 3. ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

// 01_01
// クロス積関数
Vector3 Cross(const Vector3& a, const Vector3& b);

inline Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2) { return Add(m1, m2); }
inline Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2) { return Subtract(m1, m2); }
inline Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2) { return Multiply(m1, m2); }
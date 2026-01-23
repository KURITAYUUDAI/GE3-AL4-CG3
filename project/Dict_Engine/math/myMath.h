#pragma once
#include <cmath>
#include <algorithm>
#include <stdlib.h>
#include <assert.h>
#include <numbers>
#include <vector>

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

static const float kDeltaTime = 1.0f / 60.0f;

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

// 3. 座標変換
Vector3 TransformPosition(const Vector3& vector, const Matrix4x4& matrix);

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

inline Vector3 operator+(const Vector3& v1, const Vector3& v2) { return Add(v1, v2); }
inline Vector3 operator-(const Vector3& v1, const Vector3& v2) { return Subtract(v1, v2); }
inline Vector3 operator*(float s, const Vector3& v) { return Multiply(s, v); }
inline Vector3 operator*(const Vector3& v, float s) { return s * v; }
inline Vector3 operator/(float s, const Vector3& v) { return Multiply(1.0f / s, v); }
inline Vector3 operator/(const Vector3& v, float s) { return s / v; }
inline Vector3 operator*(const Vector3& v1, const Vector3& v2) { return { v1.x * v2.x, v1.y * v2.y, v1.z * v2.z }; }
inline Vector3 operator/(const Vector3& v1, const Vector3& v2) { return { v1.x / v2.x, v1.y / v2.y, v1.z / v2.z }; }
inline Vector3 operator-(const Vector3& v) { return { -v.x, -v.y, -v.z }; }
inline Vector3 operator+(const Vector3& v) { return v; }

inline Vector4 operator+(const Vector4& v1, const Vector4& v2)
{
	return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w };
}

inline Vector4 operator-(const Vector4& v1, const Vector4& v2)
{
	return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w };
}

inline Vector4 operator*(float s, const Vector4& v) { return { s * v.x, s * v.y, s * v.z, s * v.w }; }

inline Vector4 operator*(const Vector4& v, float s) { return s * v; }

inline Vector4 operator/(const Vector4& v, float s) { return { v.x / s, v.y / s, v.z / s, v.w / s }; }

inline Vector3& operator+=(Vector3& lhs, Vector3 const& rhs)
{
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	lhs.z += rhs.z;
	return lhs;
}

inline Vector3& operator-=(Vector3& lhs, Vector3 const& rhs)
{
	lhs.x -= rhs.x;
	lhs.y -= rhs.y;
	lhs.z -= rhs.z;
	return lhs;
}

inline Vector3& operator*=(Vector3& lhs, Vector3 const& rhs) {
	lhs.x *= rhs.x;
	lhs.y *= rhs.y;
	lhs.z *= rhs.z;
	return lhs;
}

inline Vector3& operator/=(Vector3& lhs, Vector3 const& rhs) {
	lhs.x /= rhs.x;
	lhs.y /= rhs.y;
	lhs.z /= rhs.z;
	return lhs;
}

inline Vector3& operator*=(Vector3& v, float s) {
	v.x *= s;
	v.y *= s;
	v.z *= s;
	return v;
}

inline Vector3& operator/=(Vector3& v, float s) {
	v.x /= s;
	v.y /= s;
	v.z /= s;
	return v;
}

inline Vector2& operator+=(Vector2& lhs, Vector2 const& rhs) {
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	return lhs;
}

inline Vector2 operator+(const Vector2& v1, const Vector2& v2) { return { v1.x + v2.x, v1.y + v2.y }; }
inline Vector2 operator-(const Vector2& v1, const Vector2& v2) { return { v1.x - v2.x, v1.y - v2.y }; }
inline Vector2 operator*(float s, const Vector2& v) { return { s * v.x, s * v.y }; }
inline Vector2 operator*(const Vector2& v, float s) { return s * v; }
inline Vector2 operator*(const Vector2& v1, const Vector2& v2) { return { v1.x * v2.x, v1.y * v2.y }; }
inline Vector2 operator/(const Vector2& v, float s) { return { v.x / s, v.y / s }; }

inline Vector2 operator-(const Vector2& v) { return { -v.x, -v.y }; }
inline Vector2 operator+(const Vector2& v) { return v; }

inline Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2) { return Add(m1, m2); }
inline Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2) { return Subtract(m1, m2); }
inline Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2) { return Multiply(m1, m2); }

// Sphere構造体
struct Sphere
{
	Vector3 center; //!< 中心座標
	float radius;   //!< 半径
};

// 直線
struct Line
{
	Vector3 origin; //!< 始点
	Vector3 diff;   //!< 終点への差分ベクトル
};

// 半直線
struct Ray
{
	Vector3 origin; //!< 始点
	Vector3 diff;   //!< 終点への差分ベクトル
};

// 線分
struct Segment
{
	Vector3 origin; //!< 始点
	Vector3 diff;   //!< 終点への差分ベクトル
};

struct Plane
{
	Vector3 normal; //!< 法線
	float distance; //!< 距離
};

struct Triangle
{
	Vector3 vertices[3]; //!< 頂点
};

struct AABB
{
	Vector3 min; //!< 最小点
	Vector3 max; //!< 最大点
};

struct OBB
{
	Vector3 center;          //!< 中心点
	Vector3 orientations[3]; //!< 座標軸。正規化・直行必須
	Vector3 size;            //!< 座標軸方向の長さの半分。中心点から面までの距離
};


/// <summary>
/// 指定した座標を中心とする正三角形の頂点を計算する関数
/// </summary>
/// <param name="center">正三角形の中心座標</param>
/// <param name="sideLength">正三角形の辺の長さ</param>
/// <param name="vertices">正三角形の頂点を格納する配列</param>
void MakeRegularTriangle(const Vector3& center, float sideLength, Vector3 vertices[3]);

void RotateAngleCorrection(float& rotate);

Vector3 Project(const Vector3& v1, const Vector3& v2);

Vector3 ClosestPoint(const Vector3& point, const Segment& segment);

// 球同士の当たり判定を求める関数
bool IsCollision(const Sphere& sphere1, const Sphere& sphere2);

// 球と平面の当たり判定を求める関数
bool IsCollision(const Sphere& sphere, const Plane& plane);

bool IsCollision(const Plane& plane, const Line& line);

bool IsCollision(const Plane& plane, const Ray& ray);

// 線分と平面の衝突判定を求める関数
bool IsCollision(const Plane& plane, const Segment& segment);

bool IsCollision(const Triangle& triangle, const Segment& segment);

bool IsCollision(const AABB& aabb, const Vector3& point);

bool IsCollision(const AABB& aabb1, const AABB& aabb2);

bool IsCollision(const AABB& aabb, const Sphere& sphere);

bool IsCollision(const AABB& aabb, const Segment& segment);

bool IsCollision(const OBB& obb, const Sphere& sphere);

bool IsCollision(const OBB& obb, const Segment& segment);

bool IsCollision(const OBB (&obb)[2]);

float Larp(const float& n1, const float& n2, const float& t);

float EaseOut(const float& n1, const float& n2, const float& t);

Matrix4x4 MakeLookRotation(const Vector3& forward, const Vector3& up);

Vector3 GetEulerFromMatrix(const Matrix4x4& m);

Matrix4x4 MakeLookRotationXAxis(const Vector3& xAxis, const Vector3& up);

// ベクトル変換
Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);

Matrix4x4 MakeLookAtMatrix(const Vector3& eye, const Vector3& target, const Vector3& up);
#include "PrimitiveManager.h"
#include "DirectXBase.h"
#include "Model.h"
#include "ModelUtility.h"

std::unique_ptr<PrimitiveManager> PrimitiveManager::instance_ = nullptr;

PrimitiveManager* PrimitiveManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = std::make_unique<PrimitiveManager>(ConstructorKey());
	}
	return instance_.get();
}

void PrimitiveManager::Finalize()
{
	instance_.reset();

}

void PrimitiveManager::Initialize()
{

}

void PrimitiveManager::CreateRing(const std::string& name, const RingConfig& config)
{
	if (ModelManager::GetInstance()->FindModel("ring_primitive") != nullptr)
	{
		return;
	}

	const uint32_t kRingDivide = config.segments;
	/*const float kOuterRadius = config.outerRadius;
	const float kInnerRadius = config.innerRadius;*/
	const float angleRenge = config.endAngle - config.startAngle;
	const float radianPerDivide = angleRenge / float(kRingDivide);

	Mesh mesh;

	for (uint32_t index = 0; index < kRingDivide; ++index)
	{
		float angle0 = config.startAngle + index * radianPerDivide;
		float angle1 = config.startAngle + (index + 1) * radianPerDivide;

		// 正規化位置
		float t0 = float(index) / float(config.segments);
		float t1 = float(index + 1) / float(config.segments);

		// 半径をサンプリング
		auto [innerRadius0, outerRadius0] = config.radiusPoints.empty()
			? std::make_pair(config.innerRadius, config.outerRadius)
			: SampleRadius(config.radiusPoints, t0);

		auto [innerRadius1, outerRadius1] = config.radiusPoints.empty()
			? std::make_pair(config.innerRadius, config.outerRadius)
			: SampleRadius(config.radiusPoints, t1);

		float sin = std::sinf(angle0);
		float cos = std::cosf(angle0);
		float sinNext = std::sinf(angle1);
		float cosNext = std::cosf(angle1);
		float u = (float(index) / float(kRingDivide)) * config.uvScaleU;
		float uNext = (float(index + 1) / float(kRingDivide)) * config.uvScaleU;
		float vOuter = 0.0f * config.uvScaleV;
		float vInner = 1.0f * config.uvScaleV;

		VertexData outerA = { {-sin * outerRadius0, cos * outerRadius0, 0.0f, 1.0f}, {u, vOuter}, {} };
		VertexData outerB = { {-sinNext * outerRadius1, cosNext * outerRadius1, 0.0f, 1.0f}, {uNext, vOuter}, {} };
		VertexData innerA = { {-sin * innerRadius0, cos * innerRadius0, 0.0f, 1.0f}, {u, vInner}, {} };
		VertexData innerB = { {-sinNext * innerRadius1, cosNext * innerRadius1, 0.0f, 1.0f}, {uNext, vInner}, {} };

		// アルファ計算（t0 と t1 それぞれで計算）
		auto CalcAlpha = [&](float t) -> float{
			float alpha = 1.0f;

			// 開始側フェード
			if (config.startFadeRange > 0.0f && t < config.startFadeRange)
			{
				alpha *= t / config.startFadeRange;
			}
			// 終了側フェード
			if (config.endFadeRange > 0.0f && t > 1.0f - config.endFadeRange)
			{
				alpha *= (1.0f - t) / config.endFadeRange;
			}

			return std::clamp(alpha, 0.0f, 1.0f);
		};

		float alpha0 = CalcAlpha(t0);
		float alpha1 = CalcAlpha(t1);

		outerA.color = { config.outerColor.x, config.outerColor.y, config.outerColor.z,
					 config.outerColor.w * alpha0 };
		outerB.color = { config.outerColor.x, config.outerColor.y, config.outerColor.z,
						 config.outerColor.w * alpha1 };
		innerA.color = { config.innerColor.x, config.innerColor.y, config.innerColor.z,
						 config.innerColor.w * alpha0 };
		innerB.color = { config.innerColor.x, config.innerColor.y, config.innerColor.z,
						 config.innerColor.w * alpha1 };

		mesh.vertices.push_back(outerA);
		mesh.vertices.push_back(innerA);
		mesh.vertices.push_back(outerB);

		mesh.vertices.push_back(outerB);
		mesh.vertices.push_back(innerA);
		mesh.vertices.push_back(innerB);
	}

	mesh.material.textureFilePath = "";
	mesh.material.color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	mesh.material.textureIndex = 0;

	ModelManager::GetInstance()->InsertMesh("ring_primitive", mesh);
}

float PrimitiveManager::CatmullRom(float p0, float p1, float p2, float p3, float t)
{
	return 0.5f * (
	   (2.0f * p1) +
	   (-p0 + p2) * t +
	   (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t * t +
	   (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t * t * t
   );
}

std::pair<float, float> PrimitiveManager::SampleRadius(
	const std::vector<PrimitiveManager::RadiusControlPoint>& points, float t)
{
	// 制御点が1点以下はそのまま返す
	if (points.size() == 1)
		return { points[0].innerRadius, points[0].outerRadius };

	// t が範囲外ならクランプ
	if (t <= points.front().t)
		return { points.front().innerRadius, points.front().outerRadius };
	if (t >= points.back().t)
		return { points.back().innerRadius, points.back().outerRadius };

	// t が属するセグメントを探す
	size_t i = 1;
	while (i < points.size() - 1 && points[i].t < t) { ++i; }

	// セグメント内の局所 t（0〜1）
	float segT = (t - points[i - 1].t) / (points[i].t - points[i - 1].t);

	// Catmull-Rom 用に端点を延長（幽霊点）
	size_t i0 = (i >= 2) ? i - 2 : 0;
	size_t i1 = i - 1;
	size_t i2 = i;
	size_t i3 = (i + 1 < points.size()) ? i + 1 : points.size() - 1;

	float inner = CatmullRom(
		points[i0].innerRadius, points[i1].innerRadius,
		points[i2].innerRadius, points[i3].innerRadius, segT);

	float outer = CatmullRom(
		points[i0].outerRadius, points[i1].outerRadius,
		points[i2].outerRadius, points[i3].outerRadius, segT);

	return { inner, outer };
}





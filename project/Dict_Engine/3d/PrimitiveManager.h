#pragma once
#include "DirectXBase.h"
#include "myMath.h"

#include <memory>
#include <cstdint>

class PrimitiveManager
{
public:
	// シングルトンインスタンスの取得
	static PrimitiveManager* GetInstance();
	// 終了
	void Finalize();

	// コンストラクタに渡すための鍵
	class ConstructorKey
	{
	private:
		ConstructorKey() = default;
		friend class PrimitiveManager;
	};

	// PassKeyを受け取るコンストラクタ
	explicit PrimitiveManager(ConstructorKey){}

private:	// シングルトン化

	static std::unique_ptr<PrimitiveManager> instance_;

	~PrimitiveManager() = default;
	PrimitiveManager(PrimitiveManager&) = delete;
	PrimitiveManager& operator=(PrimitiveManager&) = delete;

	friend struct std::default_delete<PrimitiveManager>;

public:

	struct RadiusControlPoint
	{
		float t;           // 0.0(startAngle) 〜 1.0(endAngle) の正規化位置
		float innerRadius; // その位置での内径
		float outerRadius; // その位置での外径
	};


	struct RingConfig
	{
		// 分割数
		uint32_t segments = 32;

		// 半径制御点
		std::vector<RadiusControlPoint> radiusPoints;

		// 半径
		float innerRadius = 0.2f;
		float outerRadius = 1.0f;

		// 角度範囲（ラジアン、デフォルトは全周）
		float startAngle = 0.0f;
		float endAngle = 2.0f * pi;

		// UV スケール
		float uvScaleU = 1.0f;
		float uvScaleV = 1.0f;

		// 頂点カラー（内側・外側）
		Vector4 outerColor = {1.0f, 1.0f, 1.0f, 1.0f};
		Vector4 innerColor = {1.0f, 1.0f, 1.0f, 1.0f};

		// アルファフェード（開始・終了地点）
		float startAlpha = 1.0f;   // 開始角度付近のアルファ
		float endAlpha = 1.0f;   // 終了角度付近のアルファ
		float startFadeRange = 0.0f;   // フェードが始まる割合 (0.0〜0.5)
		float endFadeRange = 0.0f;
	};

public:

	void Initialize();

	void CreateRing(const std::string& name, const RingConfig& config = {});

private:

	static float CatmullRom(float p0, float p1, float p2, float p4, float t);

	static std::pair<float, float> SampleRadius(const std::vector<RadiusControlPoint>& points, float t);

};


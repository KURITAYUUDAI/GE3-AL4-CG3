#include "SeedManager.h"

std::unique_ptr<SeedManager, SeedManager::Deleter> SeedManager::instance_ = nullptr;

SeedManager* SeedManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_.reset(new SeedManager());
	}

	return instance_.get();
}

void SeedManager::Finalize()
{
	instance_.reset();
}

void SeedManager::Initialize()
{
	// 乱数エンジンの初期化
	std::random_device randomEngine;
	randomEngine_ = std::mt19937(randomEngine());
}

int SeedManager::GenerateInt(int min, int max)
{
	// 範囲が逆転していたら交換
	if (min > max)
	{ 
		std::swap(min, max);
	}

	std::uniform_int_distribution<int> dist(min, max);
	return dist(randomEngine_);
}

float SeedManager::GenerateFloat(float min, float max)
{
	// 範囲が逆転していたら交換
	if (min > max)
		std::swap(min, max);

	// 浮動小数点の一様分布で乱数生成
	std::uniform_real_distribution<float> dist(min, max);
	return dist(randomEngine_);
}

#pragma once
#include "DirectXBase.h"
#include "object3d.h"
#include "myMath.h"
#include "Model.h"
#include <memory>

class Player
{
public:

	void Initialize();
	void Update();
	void Draw();
	void Finalize();

public:

	void SetEnvironmentTextureIndex(const uint32_t& srvIndex){ environmentTextureIndex_ = srvIndex; }

private:

	std::string psoName_ = "Environment";
	PSOManager::BlendMode blendMode_ = PSOManager::BlendMode::Normal;
	PSOManager::FillMode fillMode_ = PSOManager::FillMode::kSolid;

	std::unique_ptr<Object3d> object3d_;

	Transform transform_;

	// 環境テクスチャのsrvIndex
	uint32_t environmentTextureIndex_ = 0;

	bool isDraw_ = true;
};


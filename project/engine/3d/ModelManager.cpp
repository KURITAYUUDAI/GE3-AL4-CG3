#include "ModelManager.h"
#include "DirectXBase.h"
#include "ModelBase.h"
#include "Model.h"

ModelManager* ModelManager::instance = nullptr;

ModelManager* ModelManager::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new ModelManager;
	}
	return instance;
}

void ModelManager::Finalize()
{
	models_.clear();

	delete modelBase_;

	delete instance;
	instance = nullptr;

}

void ModelManager::LoadModel(const std::string& filePath)
{
	// 読み込み済みモデルを検索
	if (models_.contains(filePath))
	{
		// 読み込み済みなら早期return
		return;
	}

	// モデルの生成とファイル読み込み、初期化
	std::unique_ptr<Model> model = std::make_unique<Model>();
	model->Initialize(modelBase_, "resources", filePath);

	// モデルをmapコンテナに格納する
	models_.insert(std::make_pair(filePath, std::move(model)));
}

Model* ModelManager::FindModel(const std::string& filePath)
{
	// 読み込み済みモデルを検索
	if (models_.contains(filePath))
	{
		// 読み込み済みなら早期return
		return models_.at(filePath).get();
	}

	// ファイル名一致無し
	return nullptr;
}

void ModelManager::Initialize(DirectXBase* dxBase)
{
	modelBase_ = new ModelBase;
	modelBase_->Initialize(dxBase);
}

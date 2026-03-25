#include "ModelManager.h"
#include "DirectXBase.h"
#include "Model.h"

std::unique_ptr<ModelManager> ModelManager::instance_ = nullptr;

ModelManager* ModelManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = std::make_unique<ModelManager>(ConstructorKey());
	}
	return instance_.get();
}

void ModelManager::Finalize()
{
	models_.clear();

	instance_.reset();

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
	model->Initialize("resources", filePath);

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

void ModelManager::CreateSphere(const std::string& materialPath)
{


}

void ModelManager::Initialize(DirectXBase* dxBase)
{
	dxBase_ = dxBase;
}

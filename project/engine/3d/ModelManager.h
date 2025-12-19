#pragma once
#include <Windows.h>
#include <memory>
#include <map>
#include <string>

#include "ModelBase.h"

class DirectXBase;

class Model;

// モデルマネージャー
class ModelManager
{

public:

	// シングルトンインスタンスの取得
	static ModelManager* GetInstance();
	// 初期化
	void Initialize(DirectXBase* dxBase);
	// 終了
	void Finalize();

	/// <summary>
	/// モデルファイルの読み込み
	/// </summary>
	/// <param name="filePath">モデルファイルのパス</param>
	void LoadModel(const std::string& filePath);

	/// <summary>
	/// モデルの検索
	/// </summary>
	/// <param name="filePath">モデルファイルのパス</param>
	/// <returns>モデル</returns>
	Model* FindModel(const std::string& filePath);

	void CreateSphere(const std::string& materialPath);

public: // 外部入出力

	ModelBase* GetModelBase(){ return modelBase_.get(); }

private:	// シングルトン化

	// unique_ptr が delete するために使用する構造体
	struct Deleter
	{
		void operator()(ModelManager* p) const
		{
			// クラス内部のスコープなので private なデストラクタを呼べる
			delete p;
		}
	};

	// unique_ptr の型定義に Deleter を入れることでdeleteが可能になる
	static std::unique_ptr<ModelManager, Deleter> instance_;
	
	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(ModelManager&) = delete;
	ModelManager& operator=(ModelManager&) = delete;

private:	// 静的関数

	

private:

	struct DeleterModelBase
	{
		void operator()(ModelBase* p) const
		{
			// クラス内部のスコープなので private なデストラクタを呼べる
			delete p;
		}
	};

	std::unique_ptr<ModelBase, DeleterModelBase> modelBase_ = nullptr;

	// モデルデータ
	std::map<std::string, std::unique_ptr<Model>> models_;

};


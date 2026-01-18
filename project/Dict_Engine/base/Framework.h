#pragma once

#define _USE_MATH_DEFINES
#include <Windows.h>
#include <string>
#include <format>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>

//　ファイルやディレクトリに関する操作を行うライブラリ
#include <filesystem>
// ファイルに書いたり読んだりするライブラリ
#include <fstream>
// 時間を扱うライブラリ
#include <chrono>

// Debug用のあれやこれやを使えるようにする
#include <dbghelp.h>
#pragma comment(lib, "Dbghelp.lib")

#include <strsafe.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")

#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")
#include <DirectXMath.h>
using namespace DirectX;

#include "myMath.h"

#include <fstream>
#include <sstream>

#include "externals/DirectXTex/DirectXTex.h"

#include <vector>
#include <array>

#include <wrl.h>


#include "DebugCamera.h"
#include "InputManager.h"

#include "StringUtility.h"
#include "D3DResourceLeakChecker.h"
#include "WindowsAPI.h"
#include "DirectXBase.h"
#include "TextureManager.h"

#include "SpriteBase.h"
#include "Sprite.h"

#include "Object3dBase.h"
#include "Object3d.h"

#include "ModelManager.h"
#include "Model.h"

#include "Camera.h"

#include "SrvManager.h"

#include "ParticleManager.h"
#include "ParticleEmitter.h"

#include "SeedManager.h"
#include "ImGuiManager.h"
#include "SoundManager.h"

// ゲーム全体
class Dict_Framework
{
public:

	virtual ~Dict_Framework() = default;

	// 初期化
	virtual void Initialize();

	// 終了
	virtual void Finalize();

	// 更新
	virtual void Update();

	// 描画
	virtual void Draw() = 0;

	// 終了チェック
	virtual bool IsEndRequest() { return endRequest_; }

public:

	// 実行
	void Run();

public:

	static LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception);

protected:

	bool endRequest_;

protected:

#ifdef _DEBUG

	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue_ = nullptr;

#endif

	// WindowsAPIのポインタ
	std::unique_ptr<WindowsAPI> winAPI_ = nullptr;

	// DirectXBaseのポインタ
	std::unique_ptr<DirectXBase> dxBase_ = nullptr;

	// スプライトの共通処理を生成
	std::unique_ptr<SpriteBase> spriteBase_ = nullptr;

	// 3Dオブジェクトの共通処理を生成
	std::unique_ptr<Object3dBase> object3dBase_ = nullptr;

	// カメラ
	std::unique_ptr<Camera> camera_ = nullptr;


	// SRVマネージャー
	SrvManager* srvManager_ = SrvManager::GetInstance();

	// インプットマネージャー
	ImGuiManager* imguiManager_ = ImGuiManager::GetInstance();

	// テクスチャマネージャー
	TextureManager* textureManager_ = TextureManager::GetInstance();

	// パーティクルマネージャー
	ParticleManager* particleManager_ = ParticleManager::GetInstance();

	// インプットマネージャー
	InputManager* inputManager_ = InputManager::GetInstance();

private:

	D3DResourceLeakChecker LeakCheck_;
};


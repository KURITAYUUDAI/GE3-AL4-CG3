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

#include "engine/math/myMath.h"

#include <fstream>
#include <sstream>

// imGuiのinclude
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
//extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

#include "externals/DirectXTex/DirectXTex.h"

#include <vector>
#include <array>

#include <wrl.h>

#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")

#include "DebugCamera.h"
#include "Input.h"

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

#include "ParticleBase.h"
#include "Particle.h"

#include "SeedManager.h"

//class ResourceObject
//{
//public:
//	ResourceObject(ID3D12Resource* resource)
//		:resource_(resource)
//	{}
//	~ResourceObject()
//	{
//		if (resource_)
//		{
//			resource_->Release();
//		}
//	}
//	ID3D12Resource* Get() { return resource_; }
//private:
//	ID3D12Resource* resource_;
//
//};

// チャンクヘッダ
struct  ChunkHeader
{
	char id[4];	// チャンクごとのID
	int32_t size;	// チャンクサイズ
};

// RIFFヘッダチャンク
struct RiffHeader
{
	ChunkHeader chunk;	// "RIFF"
	char type[4];	// "WAVE"
};

// FMTチャンク
struct FormatChunk
{
	ChunkHeader chunk;	// "fmt "
	WAVEFORMATEX fmt;	// 波形フォーマット
};

// 音声データ
struct SoundData
{
	// 波形フォーマット
	WAVEFORMATEX wfex;
	// バッファの先頭アドレス
	BYTE* pBuffer;
	// バッファのサイズ
	unsigned int bufferSize;
};

void Log(const std::string& message)
{
	OutputDebugStringA(message.c_str());
}

//void Log(std::ostream& os, const std::string& message)
//{
//	os << message << std::endl;
//	OutputDebugStringA(message.c_str());
//}

static LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception)
{
	// 時刻を取得して、時刻を名前に入れたファイルを作成、Dumpsディレクトリ以下に出力
	SYSTEMTIME time;
	GetLocalTime(&time);
	wchar_t filePath[MAX_PATH] = { 0 };
	CreateDirectory(L"./Dumps", nullptr);
	StringCchPrintfW(filePath, MAX_PATH, L"./Dumps/%04d-%02d%02d-%02d%02d.dmp", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);
	HANDLE dumpFileHandle = CreateFile(filePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
	// processeId (このexeのId) とクラッシュ (例外) の発生したthreadIdを取得
	DWORD processId = GetCurrentProcessId();
	DWORD threadId = GetCurrentThreadId();
	// 設定情報を入力
	MINIDUMP_EXCEPTION_INFORMATION minidumpInformation{ 0 };
	minidumpInformation.ThreadId = threadId;
	minidumpInformation.ExceptionPointers = exception;
	minidumpInformation.ClientPointers = TRUE;
	// Dumpを出力。MiniDumpNormalは最低限の情報を出力するフラグ
	MiniDumpWriteDump(GetCurrentProcess(), processId, dumpFileHandle, MiniDumpNormal, &minidumpInformation, nullptr, nullptr);
	// 他に関連付けられているSEH例外ハンドラがあれば実行。通常はプロセスを終了する
	return EXCEPTION_EXECUTE_HANDLER;
}





Microsoft::WRL::ComPtr<ID3D12Resource> 
CreateModelResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, const DirectX::TexMetadata& metadata)
{
	// 1. metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width); // Textureの幅
	resourceDesc.Height = UINT(metadata.height); // Textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels); // mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize); // 奥行き　or　配列Textureの配列数
	resourceDesc.Format = metadata.format; // サンプリングカウント。1固定。
	resourceDesc.SampleDesc.Count = 1; // サンプリングカウント。1固定。
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension); // テクスチャの次元数。普段使っているのは2次元

	// 2. 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;	// 細かい設定を行う
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK; // WriteBackポリシーでCPUアクセス可能
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0; // プロセッサの近くに配置



	// 3. Resourceの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties, // Heapの設定
		D3D12_HEAP_FLAG_NONE, // Heapの特殊な設定。特になし。
		&resourceDesc, // Resourceの設定
		D3D12_RESOURCE_STATE_GENERIC_READ, // 初回のResourceState。Textureは基本読むだけ
		nullptr, // Clear最適値。使わないのでnullptr
		IID_PPV_ARGS(&resource)); // 作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));
	return resource;
}






SoundData SoundLoadWave(const char* filename)
{
	/*HRESULT result;*/

	// 1. ファイルを開く
	// ファイル入力ストリームのインスタンス
	std::ifstream file;
	// ,wavファイルをバイナリモードで開く
	file.open(filename, std::ios_base::binary);
	// ファイルが開けなかったら止める
	assert(file.is_open());
	
	// 2. .wavファイルのデータ読み込み
	// RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	// ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0)
	{
		assert(0);
	}
	// タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0)
	{
		assert(0);
	}
	// Formatチャンクの読み込み
	FormatChunk format = {};
	// チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0)
	{
		assert(0);
	}
	// チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);
	// Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	// JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK", 4) == 0)
	{
		// 読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data, sizeof(data));
	}
	if (strncmp(data.id, "data", 4) != 0)
	{
		assert(0);
	}

	// Dataチャンクのデータ部（波形データ）の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	// 3. ファイルを閉じる
	file.close();

	// 4. 読み込んだ音声データを構造体に格納して返す
	SoundData soundData = {};

	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;
	
	return soundData;
}

void SoundUnload(SoundData* soundData)
{
	// バッファのメモリを解放
	delete[] soundData->pBuffer;

	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

void SoundPlayWave(IXAudio2* xAudio2, const SoundData& soundData)
{
	HRESULT result;

	// 波形フォーマットを基にSourceVoiceの生成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
	assert(SUCCEEDED(result));

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer;
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	// 波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->Start();
}

//bool IsPress(uint8_t key)
//{
//	
//}
//
//// 押した瞬間を検出する
//bool IsTrigger(uint8_t key)
//{
//
//}
//
//// 離した瞬間を検出する
//bool IsRelease(uint8_t key)
//{
//	
//}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	

	// main関数の先頭でCOMの初期化を行う
	CoInitializeEx(0, COINIT_MULTITHREADED);

	D3DResourceLeakChecker LeakCheck;

	// 誰も捕捉しなかった場合に(Unhandled)、補足する関数を登録
	// main関数はじまってすぐに登録するといい
	SetUnhandledExceptionFilter(ExportDump);

#ifdef _DEBUG

	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		// デバッグレイヤーを有効化する
		debugController->EnableDebugLayer();
		// さらにCGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}

#endif // _DEBUG

	HRESULT hr;

	// WindowsAPIのポインタ
	WindowsAPI* winAPI = nullptr;

	// WindowsAPIの初期化
	winAPI = new WindowsAPI();
	winAPI->Initialize();

	// DirectXBaseのポインタ
	DirectXBase* dxBase = new DirectXBase();
	dxBase->Initialize(winAPI);

	SrvManager* srvManager = SrvManager::GetInstance();
	srvManager->Initialize(dxBase);

	TextureManager* textureManager = TextureManager::GetInstance();
	textureManager->SetDxBase(dxBase);

	ModelManager::GetInstance()->Initialize(dxBase);

	SeedManager::GetInstance()->Initialize();

#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
	if (SUCCEEDED(dxBase->GetDevice()->QueryInterface(IID_PPV_ARGS(&infoQueue))))
	{

		// ヤバイエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		// エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		// 警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		//// 解放
		//infoQueue->Release();
		// 抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] =
		{
			// Windows11でのDXGIデバッグレイヤーの相互作用バグによるエラーメッセージ
			// https://stackoverflow.com/questions/6980524/directx-12-application-is-crashing-in-windows-11
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		// 抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		// 指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);
	}
#endif // _DEBUG

	// --- ここから描画初期化 ---

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0; // 0から始まる
	descriptorRange[0].NumDescriptors = 1;	// 数は一つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算

	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。複数設定できるので配列。PixelShaderのMaterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameters[4] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	// CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	// PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;	// レジスタ番号0とバインド

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	// CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;	// VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0;	// レジスタ番号0とバインド

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;	// Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);	// Tableで利用する数

	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	// CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	// PixelShaderで使う
	rootParameters[3].Descriptor.ShaderRegister = 1;	// レジスタ番号1を使う

	descriptionRootSignature.pParameters = rootParameters;	// ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);	// 配列の長さ


	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;	// バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	// 0～1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;	// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;	// ありったけのMipMapを使う
	staticSamplers[0].ShaderRegister = 0;	// レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	// PixelShaderで使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);


	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr))
	{
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	/// 02_00

	// バイナリをもとに生成
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	hr = dxBase->GetDevice()->CreateRootSignature(0,
		signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));

	// InputLayout
	/*D3D12_INPUT_ELEMENT_DESC inputElementDescs[1] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);*/

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = dxBase->CompileShader(
		L"resources/shaders/Object3d.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = dxBase->CompileShader(
		L"resources/shaders/Object3d.PS.hlsl",L"ps_6_0");
	assert(pixelShaderBlob != nullptr);

	
	// スプライトの共通処理を生成
	SpriteBase* spriteBase = nullptr;
	// スプライトの共通部を初期化
	spriteBase = new SpriteBase;
	spriteBase->Initialize(dxBase);

	// 3Dオブジェクトの共通処理を生成
	Object3dBase* object3dBase = nullptr;
	// 3Dオブジェクトの共通部を初期化
	object3dBase = new Object3dBase;
	object3dBase->Initialize(dxBase);

	// パーティクルの共通処理を初期化
	ParticleBase* particleBase = nullptr;
	// パーティクルの共通部を初期化
	particleBase = new ParticleBase;
	particleBase->Initialize(dxBase);


	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();	// RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;	// InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };	// VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };		// PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;	// BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;	// RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むかの設定（気にしなくて良い）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// 実際に生成
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipeLineState = nullptr;
	hr = dxBase->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipeLineState));
	assert(SUCCEEDED(hr));



	

	Input* input = new Input;
	input->Initialize(winAPI);

	// サウンド再生エンジンをローカル変数で宣言
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	// インスタンスを生成
	hr = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr));

	// 全てのボイスが再生時に必ず通る「マスターボイス」を宣言
	IXAudio2MasteringVoice* masterVoice;
	// インスタンスを生成
	hr = xAudio2->CreateMasteringVoice(&masterVoice);
	assert(SUCCEEDED(hr));


	



	//// モデル読み込み
	//std::vector<ModelData> modelData = LoadObjFile("resources", "multiMaterial.obj");

	//// 頂点リソースを作る
	//std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> vertexResource(modelData.size());
	///*= CreateBufferResource(device, sizeof(VertexData) * modelData.vertices.size());*/

	//// 頂点バッファビューを作成する
	//std::vector<D3D12_VERTEX_BUFFER_VIEW> vertexBufferView(modelData.size());

	//// 頂点リソースにデータを書き込む
	//std::vector<VertexData*> vertexData(modelData.size());

	//// マテリアル用のリソースを作る。
	//std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> materialResource(modelData.size());

	//// マテリアルにデータを書き込む
	//std::vector<Material*> materialData(modelData.size());

	//for (size_t i = 0; i < modelData.size(); ++i)
	//{
	//	vertexResource[i] = dxBase->CreateBufferResource(sizeof(VertexData) * modelData[i].vertices.size());

	//	assert(vertexResource[i] && "CreateBufferResource failed");

	//	// リソースの先頭のアドレスから使う
	//	vertexBufferView[i].BufferLocation = vertexResource[i]->GetGPUVirtualAddress();
	//	// 使用するリソースのサイズは頂点のサイズ
	//	vertexBufferView[i].SizeInBytes = UINT(sizeof(VertexData) * modelData[i].vertices.size());
	//	// 1頂点当たりのサイズ
	//	vertexBufferView[i].StrideInBytes = sizeof(VertexData);

	//	// 書き込むためのアドレスを取得
	//	vertexResource[i]->Map(0, nullptr, reinterpret_cast<void**>(&vertexData[i]));
	//	// 頂点データをコピーする
	//	std::memcpy(vertexData[i], modelData[i].vertices.data(), sizeof(VertexData) * modelData[i].vertices.size());

	//	materialResource[i] = dxBase->CreateBufferResource(sizeof(Material));
	//	// マテリアルにデータを書き込む
	//	materialData[i] = nullptr;
	//	// 書き込むためのアドレスを取得
	//	materialResource[i]->Map(0, nullptr, reinterpret_cast<void**>(&materialData[i]));
	//	// 色の書き込み
	//	materialData[i]->color = modelData[i].material.color;
	//	materialData[i]->enableLighting = true;
	//	materialData[i]->uvTransform = MakeIdentity4x4();
	//}



	//// Sphere用のindexResourceを作成する。
	//Microsoft::WRL::ComPtr<ID3D12Resource> indexResource = CreateBufferResource(device, sizeof(uint32_t) * modelData.vertices.size());
	//// IndexBufferViewを作成する
	//D3D12_INDEX_BUFFER_VIEW indexBufferView{};
	//// リソースの先頭のアドレスから使う
	//indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	//// 使用するリソースのサイズ
	//indexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	//// 1つのIndexのサイズ
	//indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	//// IndexResourceにデータを書き込む
	//uint32_t* indexData = nullptr;
	//// 書き込むためのアドレスを取得
	//indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));

	//std::memcpy(indexData, modelData.vertices.data(), sizeof(VertexData)* modelData.vertices.size());

	//// Transformation用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	//Microsoft::WRL::ComPtr<ID3D12Resource> transformationResource = dxBase->CreateBufferResource(256);
	//// データを書き込む
	//TransformationMatrix* transformationMatrixData = nullptr;
	//// 書き込むためのアドレスを取得
	//transformationResource->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));
	//// 単位行列を書き込んでおく
	//transformationMatrixData->World = MakeIdentity4x4();
	//transformationMatrixData->WVP = MakeIdentity4x4();

	//// 平行光源用のリソースを作成
	//Microsoft::WRL::ComPtr<ID3D12Resource> DirectionalLightResource = dxBase->CreateBufferResource(sizeof(DirectionalLight));
	//// データを書き込む
	//DirectionalLight* directionalLightData = nullptr;
	//// 書き込むためのアドレスを取得
	//DirectionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	//// 平行光源のデータを書き込む
	//directionalLightData->color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	//directionalLightData->direction = Vector3{ 0.0f, -1.0f, 0.0f };
	//directionalLightData->intensity = 1.0f;


	//// SRVを作成するDescriptorHeapの場所を決める
	//D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = dxBase->GetSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	//D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = dxBase->GetSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	//UINT descriptorSize = dxBase->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//// 先頭はImGuiが使っているのでその次を使う
	//textureSrvHandleCPU.ptr += dxBase->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//textureSrvHandleGPU.ptr += dxBase->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// Textureを読んで転送する
	textureManager->LoadTexture("resources/uvChecker.png");
	textureManager->LoadTexture("resources/monsterBall.png");

	//std::vector<DirectX::ScratchImage> mipImagesMtl(modelData.size());
	//for (size_t i = 0; i < modelData.size(); ++i)
	//{
	//	mipImagesMtl[i] = dxBase->LoadTexture(modelData[i].material.textureFilePath);
	//}

	//std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> textureSrvHandlesMtl(modelData.size());
	//std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> textureResourceMtl(modelData.size());
	//for (size_t i = 0; i < modelData.size(); ++i)
	//{
	//	const DirectX::TexMetadata& metadata = mipImagesMtl[i].GetMetadata();

	//	textureResourceMtl[i] = dxBase->CreateTextureResource(metadata);

	//	// metaDataを基にSRVの設定
	//	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	//	srvDesc.Format = metadata.format;
	//	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
	//	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	//	dxBase->UploadTextureData(textureResourceMtl[i].Get(), mipImagesMtl[i]);

	//	// CPU 側（ImGui分の1 + i）
	//	auto cpuHandle = dxBase->GetSRVCPUDescriptorHandle(static_cast<uint32_t>(1 + textureNum + i));
	//	// GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSize, /*index=*/ static_cast<uint32_t>(1 + textureNum + i));
	//	// GPU 側も同様に
	//	auto gpuHandle = dxBase->GetSRVGPUDescriptorHandle(static_cast<uint32_t>(1 + textureNum + i));
	//	// GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSize, /*index=*/ static_cast<uint32_t>(1 + textureNum + i));

	//	// SRV を生成
	//	dxBase->GetDevice()->CreateShaderResourceView(textureResourceMtl[i].Get(), &srvDesc, cpuHandle);
	//	textureSrvHandlesMtl[i] = gpuHandle;
	//}

	int textureIndex = 0;
	const char* textureOptions[] = { "Checker", "monsterBall" };

	// シーン初期化始め
	Camera* camera = new Camera;
	camera->Initialize();
	camera->SetTranslate({0.0f, 0.0f, -10.0f});
	object3dBase->SetDefaultCamera(camera);
	particleBase->SetDefaultCamera(camera);

	std::vector<Sprite*> sprites;

	for (size_t i = 0; i < 2; i++)
	{
		Sprite* newSprite = new Sprite();
		newSprite->Initialize(spriteBase, "resources/uvChecker.png");
		newSprite->SetAnchorPoint(Vector2{ 0.5f, 0.5f });
		sprites.push_back(newSprite);
	}

	sprites[1]->SetTexture("resources/monsterBall.png");
	sprites[1]->SetPosition(Vector2{ 720.0f, 360.0f });
	sprites[1]->AdjustTextureSize();

	std::vector<Object3d*> object3ds;

	ModelManager::GetInstance()->LoadModel("plane.obj");
	ModelManager::GetInstance()->LoadModel("axis.obj");

	for (size_t i = 0; i < 2; i++)
	{
		Object3d* newObject3d = new Object3d();
		newObject3d->Initialize(object3dBase);
		newObject3d->SetModel("plane.obj");
		object3ds.push_back(newObject3d);
	}

	object3ds[1]->SetModel("axis.obj");
  
	std::vector<Particle*> particles;
	for (size_t i = 0; i < 1; i++)
	{
		Particle* particle = new Particle();
		particle->Initialize(particleBase, 10);
		particle->SetModel("plane.obj");
		particles.push_back(particle);

	}




	// シーン初期化終わり

	SoundData soundData1 = SoundLoadWave("Resources/Alarm01.wav");

	
	/*Transform transformSphere{ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {-3.0f, 0.0f, 0.0f} };
	Matrix4x4 worldMatrixSphere, wvpMatrixSphere;*/
	
	/*std::vector<std::array<float, 4>> materialColor;
	materialColor.resize(modelData.size());

	for (size_t i = 0; i < modelData.size(); ++i)
	{
		materialColor[i][0] = materialData[i]->color.x;
		materialColor[i][1] = materialData[i]->color.y;
		materialColor[i][2] = materialData[i]->color.z;
		materialColor[i][3] = materialData[i]->color.w;
	}
	

	float directionalLightColor[4];
	directionalLightColor[0] = directionalLightData->color.x;
	directionalLightColor[1] = directionalLightData->color.y;
	directionalLightColor[2] = directionalLightData->color.z;
	directionalLightColor[3] = directionalLightData->color.w;

	Vector3 directionLightDirection;
	directionLightDirection.x = directionalLightData->direction.x;
	directionLightDirection.y = directionalLightData->direction.y;
	directionLightDirection.z = directionalLightData->direction.z;*/

	bool isDrawSprite = false;

	bool isDrawObject3d = false;
	

	DebugCamera debugCamera;
	bool isDebugCamera = false;
	
	Vector2 mousePosition = { 0.0f, 0.0f };

	// ウィンドウの×ボタンが押されるまでループ
	while (true)
	{
		if (winAPI->ProcessMessage())
		{
			// ゲームループを抜ける
			break;
		}
		else
		{
			input->Update();

			//ImGui_ImplDX12_NewFrame();
			//ImGui_ImplWin32_NewFrame();
			//ImGui::NewFrame();

			//// ゲームの処理

			////// 開発用ImGuiの処理。実際に開発用のUIを出す場合はここをゲーム固有の処理に置き換えること。
			////ImGui::ShowDemoWindow();

			//ImGui::Begin("SpriteSetting");

			//ImGui::Checkbox("DrawSprite", &isDrawSprite);

			//if (ImGui::BeginTable("ItemsTable", 7, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
			//{
			//	ImGui::TableSetupScrollFreeze(1, 1);
			//	ImGui::TableSetupColumn("position", ImGuiTableColumnFlags_WidthFixed, 80.0f);
			//	ImGui::TableSetupColumn("rotation", ImGuiTableColumnFlags_WidthFixed, 80.0f);
			//	ImGui::TableSetupColumn("size", ImGuiTableColumnFlags_WidthFixed, 80.0f);
			//	ImGui::TableSetupColumn("color", ImGuiTableColumnFlags_WidthFixed, 180.0f);
			//	ImGui::TableSetupColumn("uvTransform", ImGuiTableColumnFlags_WidthFixed, 160.0f);
			//	ImGui::TableSetupColumn("texture", ImGuiTableColumnFlags_WidthFixed, 80.0f);
			//	ImGui::TableSetupColumn("Ops", ImGuiTableColumnFlags_WidthFixed, 50.0f);
			//	ImGui::TableHeadersRow();

			//	int to_delete = -1;
			//	size_t i = 0;

			//	for (auto it = sprites.begin(); it != sprites.end(); ++it, ++i)
			//	{
			//		Sprite* sprite = *it;
			//		if (!sprite) continue;
			//	
			//		ImGui::TableNextRow();

			//		// 要素ごとにIDスタックを分ける
			//		ImGui::PushID((void*)sprite);
			//		
			//		ImGui::TableSetColumnIndex(0);
			//		Vector2 positionSprite = sprite->GetPosition();
			//		if (ImGui::DragFloat2("##positionSprite", &positionSprite.x, 1.0f, 0.0f, 1280.0f))
			//		{
			//			sprite->SetPosition(positionSprite);
			//		}

			//		ImGui::TableSetColumnIndex(1);
			//		float rotationSprite = sprite->GetRotation();
			//		if (ImGui::SliderAngle("##rotationSprite", &rotationSprite))
			//		{
			//			sprite->SetRotation(rotationSprite);
			//		}

			//		ImGui::TableSetColumnIndex(2);
			//		Vector2 sizeSprite = sprite->GetSize();
			//		if (ImGui::DragFloat2("##sizeSprite", &sizeSprite.x, 1.0f, 0.0f, 1280.0f))
			//		{
			//			sprite->SetSize(sizeSprite);
			//		}

			//		ImGui::TableSetColumnIndex(3);
			//		Vector4 colorSprite = sprite->GetColor();
			//		if (ImGui::ColorEdit4("##colorSprite", &colorSprite.x))
			//		{
			//			sprite->SetColor(colorSprite);
			//		}

			//		ImGui::TableSetColumnIndex(4);
			//		Transform uvTransformSprite = sprite->GetUVTransform();
			//		if (ImGui::DragFloat2("##UVScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f))
			//		{
			//			sprite->SetUVScale(uvTransformSprite.scale);
			//		}
			//		if (ImGui::DragFloat2("##UVTranslate", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f))
			//		{
			//			sprite->SetUVTranslate(uvTransformSprite.translate);
			//		}
			//		if (ImGui::SliderAngle("##UVRotate", &uvTransformSprite.rotate.z))
			//		{
			//			sprite->SetUVRotate(uvTransformSprite.rotate);
			//		}
			//		Vector2 textureLeftTopSprite = sprite->GetTextureLeftTop();
			//		if(ImGui::DragFloat2("##LeftTop", &textureLeftTopSprite.x))
			//		{
			//			sprite->SetTextureLeftTop(textureLeftTopSprite);
			//		}
			//		Vector2 textureSizeSprite = sprite->GetTextureSize();
			//		if (ImGui::DragFloat2("##Size", &textureSizeSprite.x))
			//		{
			//			sprite->SetTextureSize(textureSizeSprite);
			//		}

			//		ImGui::TableSetColumnIndex(5);
			//		if (ImGui::SmallButton("UV"))
			//		{
			//			sprite->SetTexture("resources/uvChecker.png");
			//		}
			//		if (ImGui::SmallButton("MB"))
			//		{
			//			sprite->SetTexture("resources/monsterBall.png");
			//		}
			//		bool isFlipXSprite = sprite->GetIsFlipX();
			//		if (ImGui::Checkbox("FlipX", &isFlipXSprite))
			//		{
			//			sprite->SetFlipX(isFlipXSprite);
			//		}
			//		bool isFlipYSprite = sprite->GetIsFlipY();
			//		if (ImGui::Checkbox("FlipY", &isFlipYSprite))
			//		{
			//			sprite->SetFlipY(isFlipYSprite);
			//		}


			//		ImGui::TableSetColumnIndex(6);
			//		if (ImGui::SmallButton("Delete##del")) 
			//		{
			//			to_delete = int(i);
			//		}

			//		ImGui::PopID();
			//	}

			//	ImGui::EndTable();

			//	if (to_delete >= 0 && to_delete < (int) sprites.size()) 
			//	{
			//		delete sprites[to_delete];
			//		sprites.erase(sprites.begin() + to_delete);
			//	}

			//	if (ImGui::SmallButton("Add##del"))
			//	{
			//		Sprite* sprite = new Sprite();
			//		sprite->Initialize(spriteBase, "resources/uvChecker.png");
			//		sprites.push_back(sprite);
			//	}
			//}

			//ImGui::End();


			//ImGui::Begin("ModelSetting");

			//if (ImGui::BeginTable("ItemsTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
			//{
			//	ImGui::TableSetupScrollFreeze(1, 1);
			//	ImGui::TableSetupColumn("transform", ImGuiTableColumnFlags_WidthFixed, 180.0f);
			//	/*ImGui::TableSetupColumn("color", ImGuiTableColumnFlags_WidthFixed, 180.0f);*/
			//	ImGui::TableSetupColumn("model", ImGuiTableColumnFlags_WidthFixed, 80.0f);
			//	ImGui::TableSetupColumn("Ops", ImGuiTableColumnFlags_WidthFixed, 50.0f);
			//	ImGui::TableHeadersRow();

			//	int to_delete = -1;
			//	size_t i = 0;

			//	for (auto it = object3ds.begin(); it != object3ds.end(); ++it, ++i)
			//	{
			//		Object3d* object3d = *it;
			//		if (!object3d) continue;

			//		ImGui::TableNextRow();

			//		// 要素ごとにIDスタックを分ける
			//		ImGui::PushID((void*)object3d);

			//		ImGui::TableSetColumnIndex(0);
			//		Vector3 scaleModel = object3d->GetScale();
			//		if (ImGui::DragFloat3("##scaleModel", &scaleModel.x, 0.01f, 0.0f, 10.0f))
			//		{
			//			object3d->SetScale(scaleModel);
			//		}
			//		Vector3 rotateModel = object3d->GetRotate();
			//		if (ImGui::DragFloat3("##rotateModel", &rotateModel.x, (1.0f / 180.0f) * pi, -2.0f * pi, 2.0f * pi))
			//		{
			//			object3d->SetRotate(rotateModel);
			//		}
			//		Vector3 translateModel = object3d->GetTranslate();
			//		if (ImGui::DragFloat3("##positionModel", &translateModel.x, 0.01f, 0.0f, 1280.0f))
			//		{
			//			object3d->SetTranslate(translateModel);
			//		}

			//		/*ImGui::TableSetColumnIndex(1);
			//		Vector4 colorModel = object3d->GetColor();
			//		if (ImGui::ColorEdit4("##colorModel", &colorModel.x))
			//		{
			//			sprite->SetColor(colorModel);
			//		}*/

			//		ImGui::TableSetColumnIndex(1);
			//		if (ImGui::SmallButton("plane"))
			//		{
			//			object3d->SetModel("plane.obj");
			//		}
			//		if (ImGui::SmallButton("axis"))
			//		{
			//			object3d->SetModel("axis.obj");
			//		}


			//		ImGui::TableSetColumnIndex(2);
			//		if (ImGui::SmallButton("Delete##del"))
			//		{
			//			to_delete = int(i);
			//		}

			//		ImGui::PopID();
			//	}

			//	ImGui::EndTable();

			//	if (to_delete >= 0 && to_delete < (int)object3ds.size())
			//	{
			//		delete object3ds[to_delete];
			//		object3ds.erase(object3ds.begin() + to_delete);
			//	}

			//	if (ImGui::SmallButton("Add##del"))
			//	{
			//		Object3d* newObject3d = new Object3d();
			//		newObject3d->Initialize(object3dBase);
			//		newObject3d->SetModel("plane.obj");
			//		object3ds.push_back(newObject3d);
			//	}
			//}

			//ImGui::End();




			//ImGui::Begin("Window");

			//ImGui::Checkbox("DebugCamera", &isDebugCamera);

			//ImGui::Combo("Texture", &textureIndex, textureOptions, IM_ARRAYSIZE(textureOptions));



			
			//for (size_t i = 0; i < modelData.size(); ++i)
			//{
			//	std::string label = "##Color" + std::to_string(i);
			//	if (ImGui::ColorEdit4(label.c_str(), materialColor[i].data()))
			//	{
			//		// materialColorを上書きする
			//		materialData[i]->color = Vector4(materialColor[i][0], materialColor[i][1], materialColor[i][2], materialColor[i][3]);
			//		// （＝GPU 側で使われるマテリアル色を更新）
			//	}
			//}

			//ImGui::DragFloat3("DirectionalLightDirection", &directionLightDirection.x, 0.01f);


			//ImGui::DragFloat("DirectionLightIntensity", &directionalLightData->intensity, 0.01f);

			//if (ImGui::ColorEdit4("DirectionalLightColor", directionalLightColor))
			//{
			//	// directionalLightColorを上書きする
			//	directionalLightData->color = Vector4(directionalLightColor[0], directionalLightColor[1], directionalLightColor[2], directionalLightColor[3]);
			//	// （＝GPU 側で使われるマテリアル色を更新）
			//}

			//Transform cameraTransform = 
			//	Transform{ camera->GetScale(), camera->GetRotate(), camera->GetTranslate() };
			//if (ImGui::DragFloat3("CameraScale", &cameraTransform.scale.x, 0.01f))
			//{
			//	camera->SetScale(cameraTransform.scale);
			//}
			//if (ImGui::DragFloat3("CameraRotate", &cameraTransform.rotate.x, 1.0f / 180.0f * pi))
			//{
			//	camera->SetRotate(cameraTransform.rotate);
			//}
			//if (ImGui::DragFloat3("CameraTranslate", &cameraTransform.translate.x, 0.01f))
			//{
			//	camera->SetTranslate(cameraTransform.translate);
			//}

			//ImGui::Text("MouseX: %.2f, MouseY: %.2f", mousePosition.x, mousePosition.y);

			//ImGui::Text("Press 0 to play the sound");

			//ImGui::End();

			/*
			
			directionalLightData->direction = Vector3{ directionLightDirection.x, directionLightDirection.y, directionLightDirection.z };
			
			*/

			
			/*worldMatrixSphere = MakeAffineMatrix(transformSphere.scale, transformSphere.rotate, transformSphere.translate);
			wvpMatrixSphere = Multiply(worldMatrixSphere, Multiply(viewMatrix, projectionMatrix));
			transformationMatrixDataSphere->World = worldMatrixSphere;
			transformationMatrixDataSphere->WVP = wvpMatrixSphere;*/

			mousePosition.x = static_cast<float>(input->MousePoint(winAPI->GetHwnd()).x);
			mousePosition.y = static_cast<float>(input->MousePoint(winAPI->GetHwnd()).y);

			camera->Update();
			
			for (auto it = sprites.begin(); it != sprites.end(); ++it)
			{ 
				Sprite* sprite = *it;
				sprite->Update();
			}

			for (auto it = object3ds.begin(); it != object3ds.end(); ++it)
			{
				Object3d* object3d = *it;
				object3d->Update();
			}

			for (auto it = particles.begin(); it != particles.end(); ++it)
			{
				Particle* particle = *it;
				particle->Update();
			}


			//// ImGuiの内部コマンドを生成する
			//ImGui::Render();




			// 描画の処理
			dxBase->PreDraw();

			srvManager->PreDraw();

			if (isDrawObject3d)
			{
				object3dBase->DrawingCommon();

				for (size_t i = 0; i < object3ds.size(); i++)
				{
					object3ds[i]->Draw();
				}
			}


			particleBase->DrawingCommon();


			for (size_t i = 0; i < particles.size(); i++)
			{
				particles[i]->Draw();
			}

			/*for (size_t i = 0; i < particle3ds.size(); i++)
			{
				particle3ds[i]->Draw();
			}*/

			//// SphereのIndexBufferViewを設定
			//commandList->IASetIndexBuffer(&indexBufferView);
			// 
			//// Sphereの描画
			//dxBase->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewSphere);	// VBVを設定
			//// SphereのIndexBufferViewを設定
			//dxBase->GetCommandList()->IASetIndexBuffer(&indexBufferViewSphere);
			//// SphereのマテリアルのCBufferの場所を設定
			//dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResourceSphere->GetGPUVirtualAddress());
			//// SphereのTransformationMatrixCBufferの場所を設定
			//dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationResourceSphere->GetGPUVirtualAddress());
			//// SRV用のDescriptorTableの先頭を設定。2はrootParameter[2]である。
			//dxBase->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandles[0]);	// SphereはCheckerを使う
			//// 平行光源用のCBufferをバインド（rootParameter[3] = b1）
			//dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(3, DirectionalLightResource->GetGPUVirtualAddress());
			//// 描画！（DrawCall/ドローコール）。
			//dxBase->GetCommandList()->DrawIndexedInstanced(kSubdivision * kSubdivision * 6, 1, 0, 0, 0);
			

			if (isDrawSprite)
			{
				spriteBase->DrawingCommon();

				for (size_t i = 0; i < sprites.size(); i++)
				{
					sprites[i]->Draw();
				}
			}
						
			

			if (input->TriggerKey(DIK_0))
			{
				SoundPlayWave(xAudio2.Get(), soundData1);
			}


			//// 実際のcommandListのImGuiの描画コマンドを積む
			//ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxBase->GetCommandList());

			
			dxBase->PostDraw();


		}
	}

	//// ImGuiの終了処理
	//ImGui_ImplDX12_Shutdown();
	//ImGui_ImplWin32_Shutdown();
	//ImGui::DestroyContext();

	// XAudio2解放
	xAudio2.Reset();

	// 音声データ解放
	SoundUnload(&soundData1);

	for (auto it = particles.begin(); it != particles.end(); ++it)
	{
		Particle* particle = *it;
		delete particle;
	}
	particles.clear();


	for (auto it = object3ds.begin(); it != object3ds.end(); ++it)
	{
		Object3d* object3d = *it;
		delete object3d;
	}
	object3ds.clear();

	// ポインタ解放
	delete object3dBase;
	object3dBase = nullptr;


	for (auto it = sprites.begin(); it != sprites.end(); ++it)
	{
		Sprite* sprite = *it;
		delete sprite;
	}
	sprites.clear();

	// ポインタ解放
	delete spriteBase;
	spriteBase = nullptr;

	// SeedManager終了処理
	SeedManager::GetInstance()->Finalize();

	// ModelManager終了処理
	ModelManager::GetInstance()->Finalize();

	// TextureManager終了処理
	textureManager->Finalize();

	// SrvManager終了処理
	SrvManager::GetInstance()->Finalize();

	// DirectXBase終了処理
	dxBase->Finalize();

	// WindowsAPI終了処理
	winAPI->Finalize();

	delete dxBase;
	dxBase = nullptr;
	delete winAPI;
	winAPI = nullptr;
	delete input;
	input = nullptr;


	return 0;
}
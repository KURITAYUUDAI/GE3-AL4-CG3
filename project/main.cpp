#define _USE_MATH_DEFINES
#include <Windows.h>
#include <cstdint>
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

float pi = static_cast<float>(M_PI);

#include <fstream>
#include <sstream>

// imGuiのinclude
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

#include "externals/DirectXTex/DirectXTex.h"

#include <vector>
#include <array>

#include <wrl.h>

#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")

#include "engine/io/DebugCamera.h"

#include "engine/io/Input.h"

struct VertexData
{
	XMFLOAT4 position;
	XMFLOAT2 texcoord;
	XMFLOAT3 normal;
};

struct Material
{
	XMFLOAT4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
};

struct TransformationMatrix
{
	Matrix4x4 WVP;
	Matrix4x4 World;
};

struct DirectionalLight
{
	XMFLOAT4 color;		//!< ライトの色
	XMFLOAT3 direction;	//!< ライトの方向
	float intensity;	//!< 輝度
};

struct MaterialData
{
	std::string textureFilePath;
	XMFLOAT4 color;
};

struct ModelData
{
	std::vector<VertexData> vertices;	//!< 頂点データ
	MaterialData material;
};

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

struct D3DResourceLeakChecker
{
	~D3DResourceLeakChecker()
	{
		// リソースリークチェック
		Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
		{
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
			
		}
	}
};

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

std::wstring ConvertString(const std::string& str) {
	if (str.empty()) {
		return std::wstring();
	}

	auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
	if (sizeNeeded == 0) {
		return std::wstring();
	}
	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
	return result;
}

std::string ConvertString(const std::wstring& str) {
	if (str.empty()) {
		return std::string();
	}

	auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
	if (sizeNeeded == 0) {
		return std::string();
	}
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
	return result;
}

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

Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
	// CompilerするShaderファイルへのパス
	const std::wstring& filePath,
	// Compilerに使用するProfile
	const wchar_t* profile,
	// 初期化で生成したものを3つ
	const Microsoft::WRL::ComPtr<IDxcUtils>& dxcUtils,
	const Microsoft::WRL::ComPtr<IDxcCompiler3>& dxcCompiler,
	const Microsoft::WRL::ComPtr<IDxcIncludeHandler>& includeHandler)
{
	/// 1. hlslファイルを読む
	// これからシェーダーをコンパイルする旨をログに出す
	Log(ConvertString(std::format(L"Begin CompileShader, path:{}, profile:{}\n", filePath, profile)));
	// hlslファイルを読む
	Microsoft::WRL::ComPtr<IDxcBlobEncoding> shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	// 読めなかったら止める
	assert(SUCCEEDED(hr));
	// 読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;
	 
	/// 2. Compileする
	LPCWSTR arguments[] =
	{
		filePath.c_str(),	// コンパイル対象のhlslファイル名
		L"-E", L"main",		// エントリーポイントの指定。基本的にmain文以外にはしない
		L"-T", profile,		// ShaderProfileの指定
		L"-Zi", L"-Qembed_debug",	// デバッグ用の情報を埋め込む
		L"-Od",		// 最適化を外しておく
		L"-Zpr",	// メモリレイアウトは行優先
	};
	// 実際にShaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcResult> shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer,	// 読み込んだファイル
		arguments,				// コンパイルオプション
		_countof(arguments),	// コンパイルオプションの数
		includeHandler.Get(),			// includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult) // コンパイル結果
	);
	// コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));

	// 3. 警告・エラーがでてないか確認する
	Microsoft::WRL::ComPtr<IDxcBlobUtf8> shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if(shaderError != nullptr && shaderError->GetStringLength() != 0)
	{
		Log(shaderError->GetStringPointer());
		// 警告・エラーダメゼッタイ
		assert(false);
	}
	
	// 4. Compile結果を受け取って返す
	// コンパイル結果から実行用のバイナリ部分を取得
	Microsoft::WRL::ComPtr<IDxcBlob> shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	// 成功したログを出す
	Log(ConvertString(std::format(L"Compile Succeeded, path:{}, profile:{}\n", filePath, profile)));
	
	// 実行用のバイナリを返却
	return shaderBlob;
}

Microsoft::WRL::ComPtr<ID3D12Resource> 
CreateBufferResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, size_t sizeInBytes)
{
	// ===== [テキストより独自で変換したポイント] =======
	// Vector4 → XMFLOAT4
	//                                         by ChatGPT
	// ==================================================

	// リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;	// UploadHeapを使う
	// リソースの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	// バッファリソース。テクスチャの場合はまた別の設定をする
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = sizeInBytes;	// リソースのサイズ。引数より代入
	// バッファの場合はこれらは1にする決まり
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	// バッファの場合はこれにする決まり
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// 実際にリソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&resource));
	// リソースが作れなかったので起動できない
	assert(SUCCEEDED(hr));
	return resource;
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> 
CreateDescriptorHeap(
	const Microsoft::WRL::ComPtr<ID3D12Device>& device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible)
{
	// ディスクリプタ―ヒープの生成
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;	
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	// ディスクリプタヒープが作れなかったので起動できない
	assert(SUCCEEDED(hr));
	return descriptorHeap;
}

// DirectXTexを使ってTextureを読む関数
DirectX::ScratchImage LoadTexture(const std::string& filePath)
{
	// テクスチャファイルを呼んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	// DirectX::WIC_FLAGS_FORCE_SRGB : sRGB空間で作られたモノとして読む。
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	// ミップマップの作成　（MipMap : 元画像より小さなテクスチャ群）
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	// ミップマップ付きのデータを返す
	return mipImages;
}

// 読み込んだTexture情報を基にTextureResourceを作成する関数
Microsoft::WRL::ComPtr<ID3D12Resource> 
CreateTextureResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, const DirectX::TexMetadata& metadata)
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

void UploadTextureData(const Microsoft::WRL::ComPtr<ID3D12Resource>& texture, const DirectX::ScratchImage& mipImages)
{
	// Meta情報を取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	// 全MipMapについて
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel)
	{
		// MipMapLevelを指定して各Imageを取得
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
		// Textureに転送
		HRESULT hr = texture->WriteToSubresource(
			UINT(mipLevel),			
			nullptr,				// 全領域へコピー
			img->pixels,			// 元データアドレス
			UINT(img->rowPitch),	// 1ラインサイズ
			UINT(img->slicePitch)	// 1枚サイズ
		);
		assert(SUCCEEDED(hr));
	}
}

Microsoft::WRL::ComPtr<ID3D12Resource> 
CreateDepthStencilTextureResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, int32_t width, int32_t height)
{
	// 生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;	// Textureの幅
	resourceDesc.Height = height;	// Textureの高さ
	resourceDesc.MipLevels = 1;	// mipmapの数
	resourceDesc.DepthOrArraySize = 1;	// 奥行き or 配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;	// サンプリングカウント。1固定。
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;	// 2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;	// DepthStencilとして使う通知

	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;	// VRAM上に作る

	// 深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;	// 1.0f（最大値）でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// フォーマット。Resourceと合わせる

	// Resourceの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,	// Heapの設定
		D3D12_HEAP_FLAG_NONE,	// Heapの特殊な設定。特になし。
		&resourceDesc,	// Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	// 深度値を書き込む状態にしておく
		&depthClearValue,	// Clear最適値
		IID_PPV_ARGS(&resource)
	);
	assert(SUCCEEDED(hr));
	return resource;
}

D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(
	const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(
	const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename, const std::string& mtlname)
{
	// 1. 中で必要となる変数の宣言
	MaterialData materialData; // 構築するMaterialData
	std::string line; // ファイルから読んだ1行を格納するもの
	bool isLoad = false; // 引数で指定されたマテリアルの行に来た場合読み込む

	// 2. ファイルを開く
	std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
	assert(file.is_open()); // とりあえず開けなかったら止める

	// 3. 実際にファイルを読み、MaterialDataを構築していく
	while (std::getline(file, line))
	{
		std::string identifier;
		std::istringstream s(line);
		s >> identifier; // 先頭の識別子を読む

		// identifierに応じた処理
		if (identifier == "newmtl")
		{
			std::string materialName;
			s >> materialName;

			// 指定されたmtlNameと同じならisLaodをtrueにする
			if (materialName == mtlname)
			{
				isLoad = true;
			}
			else
			{
				isLoad = false;
			}
		}
		if (isLoad)
		{
			if (identifier == "map_Kd")
			{
				std::string textureFilename;
				s >> textureFilename;
				// 連結してファイルパスにする
				materialData.textureFilePath = directoryPath + "/" + textureFilename;
			}
			else if (identifier == "Kd")
			{
				s >> materialData.color.x >> materialData.color.y >> materialData.color.z;
				materialData.color.w = 1.0f;
			}
		}
	}	

	// 4. MaterialDataを返す
	return materialData;
}

std::vector<ModelData> LoadObjFile(const std::string& directoryPath, const std::string& filename)
{
	// 1. 中で必要となる変数の宣言
	std::vector<ModelData> meshes; // サブメッシュを格納する配列
	ModelData currentMesh; // 現在のメッシュデータ
	std::string materialFilename; // mtllibから取得したmtlファイル名
	std::vector<XMFLOAT4> positions; // 位置
	std::vector<XMFLOAT3> normals; // 法線
	std::vector<XMFLOAT2> texcoords; // テクスチャ座標
	std::string line; // ファイルから読んだ1行を格納するもの

	// 2. ファイルを開く
	std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
	assert(file.is_open()); // とりあえず開けなかったら止める

	// 3. 実際にファイルを読み、ModelDataを構築していく
	while (std::getline(file, line))
	{
		std::string identifier;
		std::istringstream s(line);
		s >> identifier; // 先頭の識別子を読む

		// identifierに応じた処理
		if (identifier == "o")
		{
			// これまでのcurrentMeshを保存し、新しいcurrentMeshを初期化する
			if (!currentMesh.vertices.empty() || !currentMesh.material.textureFilePath.empty())
			{
				meshes.push_back(currentMesh);
			}
			currentMesh = ModelData(); // 新しいメッシュデータを初期化
		}
		else if (identifier == "v")
		{
			XMFLOAT4 position;
			s >> position.x >> position.y >> position.z;
			
			// ===== [テキストより独自で変換したポイント] =======
			// position.x → position.z
			//                                         by ChatGPT
			// ==================================================

			
			position.x *= -1.0f;
			position.w = 1.0f;
			positions.push_back(position);
		}
		else if (identifier == "vt")
		{
			XMFLOAT2 texcoord;
			s >> texcoord.x >> texcoord.y;

			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		}
		else if (identifier == "vn")
		{
			XMFLOAT3 normal;
			s >> normal.x >> normal.y >> normal.z;

			// ===== [テキストより独自で変換したポイント] =======
			// normal.x → normal.z
			//                                         by ChatGPT
			// ==================================================

			
			normal.x *= -1.0f;
			normals.push_back(normal);
		}
		else if (identifier == "f")
		{
			VertexData triangle[3];
			// 面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex)
			{
				std::string vertexDefintion;
				s >> vertexDefintion;
				// 頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefintion);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element)
				{
					std::string index;
					std::getline(v, index, '/');	// 区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}
				// 要素へのIndexから、実際の要素の値を取得して頂点を構築する
				XMFLOAT4 position = positions[elementIndices[0] - 1];
				XMFLOAT2 texcoord = texcoords[elementIndices[1] - 1];
				XMFLOAT3 normal = normals[elementIndices[2] - 1];
				/*VertexData vertex = { position, texcoord, normal };
				modelData.vertices.push_back(vertex);*/
				triangle[faceVertex] = { position, texcoord, normal };
			}
			// 頂点を逆順で登録することで、周り順を逆にする
			currentMesh.vertices.push_back(triangle[2]);
			currentMesh.vertices.push_back(triangle[1]);
			currentMesh.vertices.push_back(triangle[0]);

		}
		else if (identifier == "mtllib")
		{
			// materialTemplateLibraryのファイルの名前を取得する
			
			s >> materialFilename;
			
		}
		else if (identifier == "usemtl")
		{
			// usemtlから使用するMaterial名を取得する
			std::string materialName;
			s >> materialName;

			// 基本的にobjファイルと同一階層にmtlファイルは存在させるので、ディレクトリ名とファイル名を渡す
			currentMesh.material = LoadMaterialTemplateFile(directoryPath, materialFilename, materialName);
		}

	}

	// 最後に残った currentMesh を押し込む
	if (!currentMesh.vertices.empty() || !currentMesh.material.textureFilePath.empty())
	{
		meshes.push_back(currentMesh);
	}

	// 4. ModelDataを返す
	return meshes;
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



// ウィンドウプロシージャ
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
	{
		return true;
	}

	// メッセージに応じてゲーム固有の処理を行う
	switch (msg)
	{
		// ウィンドウが破棄された
	case WM_DESTROY:
		// OSに対して、アプリの終了を伝える
		PostQuitMessage(0);

		return 0;
	}

	// 標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	

	// main関数の先頭でCOMの初期化を行う
	CoInitializeEx(0, COINIT_MULTITHREADED);

	D3DResourceLeakChecker LeakCheck;

	// 誰も捕捉しなかった場合に(Unhandled)、補足する関数を登録
	// main関数はじまってすぐに登録するといい
	SetUnhandledExceptionFilter(ExportDump);

	

	WNDCLASS wc{};
	// ウィンドウプロシージャ
	wc.lpfnWndProc = WindowProc;
	// ウィンドウクラス名(なんでも良い)
	wc.lpszClassName = L"CG2WindowClass";
	// インスタンスハンドル
	wc.hInstance = GetModuleHandle(nullptr);
	// カーソル
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

	// ウィンドウクラスを登録する
	RegisterClass(&wc);

	// クライアント領域のサイズ
	const int32_t kClientWidth = 1280;
	const int32_t kClientHeight = 720;

	// ウィンドウサイズを表す構造体にクライアント領域を入れる
	RECT wrc = { 0, 0, kClientWidth, kClientHeight };

	// クライアント両機を元に実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// ウィンドウの生成
	HWND hwnd = CreateWindow(
		wc.lpszClassName,
		L"CG2",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr);

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


	// ウィンドウを表示する
	ShowWindow(hwnd, SW_SHOW);

	MSG msg{};

	// DXCIファクトリーの生成
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory = nullptr;
	// HRESULTはWindows系のエラーコードであり、
	// 関数が成功したかどうかをSUCCEEDEDマクロで判定できる
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	// 初期化の根本的な部分でエラーが出た場合はプログラムが間違っているか、どうにも出来ない場合が多いのでassertにしておく
	assert(SUCCEEDED(hr));

	// 使用するアダプタ用の変数。最初にnullptrを入れておく
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter = nullptr;
	// 良い順にアダプタを頼む
	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) !=
		DXGI_ERROR_NOT_FOUND; ++i)
	{
		// アダプターの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr)); // 取得できないのは一大事
		// ソフトウェアアダプタでなければ採用！
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
		{
			// 採用したアダプタの情報をログに出力、wstringの方なので注意
			Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		useAdapter = nullptr;
	}
	// 適切なアダプタが見つからなかったので起動できない
	assert(useAdapter != nullptr);

	Microsoft::WRL::ComPtr<ID3D12Device> device = nullptr;
	// 機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0
	};

	const char* featureLevelStrings[] = { "12.2", "12.1", "12.0" };
	// 高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i)
	{
		// 採用したアダプターでデバイスを生成
		hr = D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(&device));
		// 指定したアダプターでデバイスが生成できたかを確認
		if (SUCCEEDED(hr))
		{
			// 生成できたのでログ出力を行ってループを抜ける
			Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}

	// デバイスの生成がうまくいかなかったので起動できない
	assert(device != nullptr);
	Log("Complete create D3D12Device!!!\n");// 初期化完了のログを出す

#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) 
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

	// コマンドキューを生成する
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
	// コマンドキューの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));
	// コマンドアロケータを生成する
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	// コマンドアロケータの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));

	// コマンドリストを生成する
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr,
		IID_PPV_ARGS(&commandList));
	// コマンドリストの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));

	// スワップチェーンを生成する
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = kClientWidth;		// 画面の幅。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc.Height = kClientHeight;	// 画面の高さ。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 色の形式
	swapChainDesc.SampleDesc.Count = 1;	// マルチサンプルしない
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// 描画のターゲットとして利用する
	swapChainDesc.BufferCount = 2;	// ダブルバッファ
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	// モニタにうつしたら、中身を破棄
	// コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue.Get(), hwnd, &swapChainDesc, nullptr, nullptr, 
		reinterpret_cast<IDXGISwapChain1**>(swapChain.GetAddressOf()));
	assert(SUCCEEDED(hr));

	// RTV用のヒープでディスクリプタの数は2、RTVはShader内で触るものではないので、ShaderVisibleはfalse
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);

	// SRV用のヒープでディスクリプタの数は128、SRVはShader内で触るものなので、ShaderVisibleはtrue
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

	// SwapChainからResourceを引っ張ってくる
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources[2] = { nullptr };
	hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
	// うまく取得できなければ起動できない
	assert(SUCCEEDED(hr));
	hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
	assert(SUCCEEDED(hr));

	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	// 出力結果をSRGBに変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;	// 2Dテクスチャとして書き込む

	// RTVを2つ作るのでディスクリプタを2つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	// まず1つ目を作る。1つ目は最初のところに作るのでIndexは0に。
	rtvHandles[0] = GetCPUDescriptorHandle(rtvDescriptorHeap, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV), 0);
	device->CreateRenderTargetView(swapChainResources[0].Get(), &rtvDesc, rtvHandles[0]);
	// 2つ目を作る。Indexは0の次なので1。
	rtvHandles[1] = GetCPUDescriptorHandle(rtvDescriptorHeap, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV), 1);
	device->CreateRenderTargetView(swapChainResources[1].Get(), &rtvDesc, rtvHandles[1]);

	Input* input = new Input;
	input->Initialize(wc.hInstance, hwnd);

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



	// --- ここから描画初期化 ---


	// 初期値0でFenceを作る
	Microsoft::WRL::ComPtr<ID3D12Fence> fence = nullptr;
	uint64_t fenceValue = 0;
	hr = device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(hr));

	// FenceのSignalを待つためのイベントを作成する
	HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);

	// dxCompilerを初期化
	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils = nullptr;

	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler = nullptr;

	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));

	// 現時点ではincludeはしないが、includeに対応するための設定を行っておく
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler = nullptr;

	hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(hr));

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
	hr = device->CreateRootSignature(0,
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
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = CompileShader(L"resources/shaders/Object3D.Vs.hlsl",
		L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = CompileShader(L"resources/shaders/Object3D.PS.hlsl",
		L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(pixelShaderBlob != nullptr);

	// DepthStencilTextureをウィンドウのサイズで作成
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource = CreateDepthStencilTextureResource(device, kClientWidth, kClientHeight);

	// DSV用のヒープディスクリプタの数は1。DSVはShader内で飾るものではないので、ShaderVisibleはfalse
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

	// DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// Format。基本的にはResourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;	// 2dTexture
	// DSVHeapの先頭にDSVをつくる
	device->CreateDepthStencilView(depthStencilResource.Get(), &dsvDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());


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
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipeLineState));
	assert(SUCCEEDED(hr));

	

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDescSprite{};

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDescSprite{};
	// Depthの機能を有効化する
	depthStencilDescSprite.DepthEnable = false;
	// 書き込みします
	depthStencilDescSprite.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	// DepthStencilの設定
	graphicsPipelineStateDescSprite.DepthStencilState = depthStencilDescSprite;
	graphicsPipelineStateDescSprite.DSVFormat = DXGI_FORMAT_UNKNOWN;

	graphicsPipelineStateDescSprite.pRootSignature = rootSignature.Get();	// RootSignature
	graphicsPipelineStateDescSprite.InputLayout = inputLayoutDesc;	// InputLayout
	graphicsPipelineStateDescSprite.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };	// VertexShader
	graphicsPipelineStateDescSprite.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };		// PixelShader
	graphicsPipelineStateDescSprite.BlendState = blendDesc;	// BlendState
	graphicsPipelineStateDescSprite.RasterizerState = rasterizerDesc;	// RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDescSprite.NumRenderTargets = 1;
	graphicsPipelineStateDescSprite.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDescSprite.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むかの設定（気にしなくて良い）
	graphicsPipelineStateDescSprite.SampleDesc.Count = 1;
	graphicsPipelineStateDescSprite.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// 実際に生成
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipeLineStateSprite = nullptr;
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDescSprite,
		IID_PPV_ARGS(&graphicsPipeLineStateSprite));
	assert(SUCCEEDED(hr));

	// ===== [テキストより独自で変換したポイント] =======
	// Vector4 → XMFLOAT4
	//                                         by ChatGPT
	// ==================================================

	// モデル読み込み
	std::vector<ModelData> modelData = LoadObjFile("resources", "multiMaterial.obj");

	// 頂点リソースを作る
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> vertexResource(modelData.size());
	/*= CreateBufferResource(device, sizeof(VertexData) * modelData.vertices.size());*/

	// 頂点バッファビューを作成する
	std::vector<D3D12_VERTEX_BUFFER_VIEW> vertexBufferView(modelData.size());

	// 頂点リソースにデータを書き込む
	std::vector<VertexData*> vertexData(modelData.size());

	// マテリアル用のリソースを作る。
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> materialResource(modelData.size());

	// マテリアルにデータを書き込む
	std::vector<Material*> materialData(modelData.size());

	for (size_t i = 0; i < modelData.size(); ++i)
	{
		vertexResource[i] = CreateBufferResource(device, sizeof(VertexData) * modelData[i].vertices.size());
		// リソースの先頭のアドレスから使う
		vertexBufferView[i].BufferLocation = vertexResource[i]->GetGPUVirtualAddress();
		// 使用するリソースのサイズは頂点のサイズ
		vertexBufferView[i].SizeInBytes = UINT(sizeof(VertexData) * modelData[i].vertices.size());
		// 1頂点当たりのサイズ
		vertexBufferView[i].StrideInBytes = sizeof(VertexData);

		// 書き込むためのアドレスを取得
		vertexResource[i]->Map(0, nullptr, reinterpret_cast<void**>(&vertexData[i]));
		// 頂点データをコピーする
		std::memcpy(vertexData[i], modelData[i].vertices.data(), sizeof(VertexData) * modelData[i].vertices.size());

		materialResource[i] = CreateBufferResource(device, sizeof(Material));
		// マテリアルにデータを書き込む
		materialData[i] = nullptr;
		// 書き込むためのアドレスを取得
		materialResource[i]->Map(0, nullptr, reinterpret_cast<void**>(&materialData[i]));
		// 色の書き込み
		materialData[i]->color = modelData[i].material.color;
		materialData[i]->enableLighting = true;
		materialData[i]->uvTransform = MakeIdentity4x4();
	}



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

	// Transformation用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationResource = CreateBufferResource(device, 256);
	// データを書き込む
	TransformationMatrix* transformationMatrixData = nullptr;
	// 書き込むためのアドレスを取得
	transformationResource->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));
	// 単位行列を書き込んでおく
	transformationMatrixData->World = MakeIdentity4x4();
	transformationMatrixData->WVP = MakeIdentity4x4();

	// 平行光源用のリソースを作成
	Microsoft::WRL::ComPtr<ID3D12Resource> DirectionalLightResource = CreateBufferResource(device, sizeof(DirectionalLight));
	// データを書き込む
	DirectionalLight* directionalLightData = nullptr;
	// 書き込むためのアドレスを取得
	DirectionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	// 平行光源のデータを書き込む
	directionalLightData->color = XMFLOAT4{ 1.0f, 1.0f, 1.0f, 1.0f };
	directionalLightData->direction = XMFLOAT3{ 0.0f, -1.0f, 0.0f };
	directionalLightData->intensity = 1.0f;

	/// 球の作成

	const int kSubdivision = 16;
	int sphereSize = kSubdivision * kSubdivision * 4; // 1つの経度分割で4つの頂点が必要

	
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSphere = CreateBufferResource(device, sizeof(VertexData) * sphereSize);

	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSphere{};
	// リソースの先頭のアドレスから使う
	vertexBufferViewSphere.BufferLocation = vertexResourceSphere->GetGPUVirtualAddress();
	// 使用するリソースのサイズ
	vertexBufferViewSphere.SizeInBytes = sizeof(VertexData) * kSubdivision * kSubdivision * 4;
	// 1頂点当たりのサイズ
	vertexBufferViewSphere.StrideInBytes = sizeof(VertexData);

	// 頂点リソースにデータを書き込む
	VertexData* vertexDataSphere = nullptr;
	// 書き込むためのアドレスを取得
	vertexResourceSphere->Map(0, nullptr,
		reinterpret_cast<void**>(&vertexDataSphere));

	// 経度分割1つ分の角度 Φ
	const float kLonEvery = pi * 2.0f / static_cast<float>(kSubdivision);
	// 緯度分割1つ分の角度 Θ
	const float kLatEvery = pi / static_cast<float>(kSubdivision);
	// 緯度の方向に分割
	for (int latIndex = 0; latIndex < kSubdivision; ++latIndex)
	{
		float lat = -pi / 2.0f + kLatEvery * latIndex;
		// 経度の方向に分割しながら線を描く
		for (int lonIndex = 0; lonIndex < kSubdivision; ++lonIndex)
		{
			uint32_t start = (latIndex * kSubdivision + lonIndex) * 4;
			float lon = lonIndex * kLonEvery; // Φ
			// 頂点にデータを入力する。基準点a
			vertexDataSphere[start].position =
			{
				std::cos(lat) * std::cos(lon),
				std::sin(lat),
				std::cos(lat) * std::sin(lon),
				1.0f,
			};
			vertexDataSphere[start].texcoord =
			{
				static_cast<float>(lonIndex) / static_cast<float>(kSubdivision),
				1.0f - static_cast<float>(latIndex) / static_cast<float>(kSubdivision),
			};
			vertexDataSphere[start].normal.x = vertexDataSphere[start].position.x;
			vertexDataSphere[start].normal.y = vertexDataSphere[start].position.y;
			vertexDataSphere[start].normal.z = vertexDataSphere[start].position.z;

			vertexDataSphere[start + 1].position =
			{
				std::cos(lat + kLatEvery) * std::cos(lon),
				std::sin(lat + kLatEvery),
				std::cos(lat + kLatEvery) * std::sin(lon),
				1.0f,
			};
			vertexDataSphere[start + 1].texcoord =
			{
				static_cast<float>(lonIndex) / static_cast<float>(kSubdivision),
				1.0f - static_cast<float>(latIndex + 1) / static_cast<float>(kSubdivision),
			};
			vertexDataSphere[start + 1].normal.x = vertexDataSphere[start + 1].position.x;
			vertexDataSphere[start + 1].normal.y = vertexDataSphere[start + 1].position.y;
			vertexDataSphere[start + 1].normal.z = vertexDataSphere[start + 1].position.z;

			vertexDataSphere[start + 2].position =
			{
				std::cos(lat) * std::cos(lon + kLonEvery),
				std::sin(lat),
				std::cos(lat) * std::sin(lon + kLonEvery),
				1.0f,
			};
			vertexDataSphere[start + 2].texcoord =
			{
				static_cast<float>(lonIndex + 1) / static_cast<float>(kSubdivision),
				1.0f - static_cast<float>(latIndex) / static_cast<float>(kSubdivision),
			};
			vertexDataSphere[start + 2].normal.x = vertexDataSphere[start + 2].position.x;
			vertexDataSphere[start + 2].normal.y = vertexDataSphere[start + 2].position.y;
			vertexDataSphere[start + 2].normal.z = vertexDataSphere[start + 2].position.z;



			vertexDataSphere[start + 3].position =
			{
				std::cos(lat + kLatEvery) * std::cos(lon + kLonEvery),
				std::sin(lat + kLatEvery),
				std::cos(lat + kLatEvery) * std::sin(lon + kLonEvery),
				1.0f,
			};
			vertexDataSphere[start + 3].texcoord =
			{
				static_cast<float>(lonIndex + 1) / static_cast<float>(kSubdivision),
				1.0f - static_cast<float>(latIndex + 1) / static_cast<float>(kSubdivision),
			};
			vertexDataSphere[start + 3].normal.x = vertexDataSphere[start + 3].position.x;
			vertexDataSphere[start + 3].normal.y = vertexDataSphere[start + 3].position.y;
			vertexDataSphere[start + 3].normal.z = vertexDataSphere[start + 3].position.z;
		}
	}

	// Sphere用のindexResourceを作成する。
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource = CreateBufferResource(device, sizeof(uint32_t) * kSubdivision * kSubdivision * 6);
	// IndexBufferViewを作成する
	D3D12_INDEX_BUFFER_VIEW indexBufferViewSphere{};
	// リソースの先頭のアドレスから使う
	indexBufferViewSphere.BufferLocation = indexResource->GetGPUVirtualAddress();
	// 使用するリソースのサイズ
	indexBufferViewSphere.SizeInBytes = sizeof(uint32_t) * kSubdivision * kSubdivision * 6;
	// 1つのIndexのサイズ
	indexBufferViewSphere.Format = DXGI_FORMAT_R32_UINT;
	// IndexResourceにデータを書き込む
	uint32_t* indexData = nullptr;
	// 書き込むためのアドレスを取得
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));

	// Indexは6つの三角形で1つの四角形を表すので、6つずつ書き込む
	for (int latIndex = 0; latIndex < kSubdivision; ++latIndex)
	{
		for (int lonIndex = 0; lonIndex < kSubdivision; ++lonIndex)
		{
			// Vertexの読み込み開始位置
			uint32_t vertexStart = (latIndex * kSubdivision + lonIndex) * 4;
			// Indexの書き込み開始位置
			uint32_t indexStart = (latIndex * kSubdivision + lonIndex) * 6;

			indexData[indexStart + 0] = vertexStart + 0;
			indexData[indexStart + 1] = vertexStart + 1;
			indexData[indexStart + 2] = vertexStart + 2;
			indexData[indexStart + 3] = vertexStart + 1;
			indexData[indexStart + 4] = vertexStart + 3;
			indexData[indexStart + 5] = vertexStart + 2;
		}
	}



	// マテリアル用のリソースを作る。
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResourceSphere = CreateBufferResource(device, sizeof(Material));
	// マテリアルにデータを書き込む
	Material* materialDataSphere = nullptr;
	// 書き込むためのアドレスを取得
	materialResourceSphere->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSphere));
	// 色の書き込み
	materialDataSphere->color = XMFLOAT4{ 1.0f, 1.0f, 1.0f, 1.0f };
	materialDataSphere->enableLighting = true;
	materialDataSphere->uvTransform = MakeIdentity4x4();

	// Transformation用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationResourceSphere = CreateBufferResource(device, sizeof(TransformationMatrix));
	// データを書き込む
	TransformationMatrix* transformationMatrixDataSphere = nullptr;
	// 書き込むためのアドレスを取得
	transformationResourceSphere->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSphere));
	// 単位行列を書き込んでおく
	transformationMatrixDataSphere->World = MakeIdentity4x4();
	transformationMatrixDataSphere->WVP = MakeIdentity4x4();



	
	



	// Sprite用の頂点リソースを作る。indexResourceを使うので頂点数は4。
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSprite = CreateBufferResource(device, sizeof(VertexData) * 4);

	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};

	// リソースの先頭のアドレスから使う
	vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点4つ分のサイズ
	vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 4;
	// 1頂点当たりのサイズ
	vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);
	
	VertexData* vertexDataSprite = nullptr;
	vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));
	// 1枚目の三角形
	vertexDataSprite[0].position = { 0.0f, 360.0f, 0.0f, 1.0f };
	vertexDataSprite[0].texcoord = { 0.0f, 1.0f };
	vertexDataSprite[0].normal = { 0.0f, 0.0f, -1.0f };
	vertexDataSprite[1].position = { 0.0f, 0.0f, 0.0f, 1.0f };
	vertexDataSprite[1].texcoord = { 0.0f, 0.0f };
	vertexDataSprite[1].normal = { 0.0f, 0.0f, -1.0f };
	vertexDataSprite[2].position = { 640.0f, 360.0f, 0.0f, 1.0f };
	vertexDataSprite[2].texcoord = { 1.0f, 1.0f };
	vertexDataSprite[2].normal = { 0.0f, 0.0f, -1.0f };
	vertexDataSprite[3].position = { 640.0f, 0.0f, 0.0f, 1.0f };
	vertexDataSprite[3].texcoord = { 1.0f, 0.0f };
	vertexDataSprite[3].normal = { 0.0f, 0.0f, -1.0f };
	
	// Sprite用のindexResourceを作成する。
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResourceSprite = CreateBufferResource(device, sizeof(uint32_t) * 6);

	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};
	// リソースの先頭のアドレスから使う
	indexBufferViewSprite.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();
	// 使用するリソースのサイズはインデックス6つ分のサイズ
	indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
	// インデックスはuint32_tとする
	indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;

	uint32_t* indexDataSprite = nullptr;
	indexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
	indexDataSprite[0] = 0;		indexDataSprite[1] = 1;		indexDataSprite[2] = 2;
	indexDataSprite[3] = 1;		indexDataSprite[4] = 3;		indexDataSprite[5] = 2;

	// Sprite用のマテリアル用のリソースを作る。
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResourceSprite = CreateBufferResource(device, sizeof(Material));
	// マテリアルにデータを書き込む
	Material* materialDataSprite = nullptr;
	// 書き込むためのアドレスを取得
	materialResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSprite));
	// 色の書き込み
	materialDataSprite->color = XMFLOAT4{ 1.0f, 1.0f, 1.0f, 1.0f };
	materialDataSprite->enableLighting = false;
	materialDataSprite->uvTransform = MakeIdentity4x4();

	// Sprite用のTransformationMatrix用のリソースを作る。Matrix4x4　１つ分のサイズを用意する。
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResourceSprite = CreateBufferResource(device, 256);
	// データを書き込む
	TransformationMatrix* transformationMatrixDataSprite = nullptr;
	// 書き込むためのアドレスを取得
	transformationMatrixResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));
	// 単位行列を書き込んでおく
	transformationMatrixDataSprite->World = MakeIdentity4x4();
	transformationMatrixDataSprite->WVP = MakeIdentity4x4();


	// SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	UINT descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// 先頭はImGuiが使っているのでその次を使う
	textureSrvHandleCPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleGPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// Textureを読んで転送する
	DirectX::ScratchImage mipImages[2];
	mipImages[0] = LoadTexture("resources/uvChecker.png");
	mipImages[1] = LoadTexture("resources/monsterBall.png");

	const int textureNum = 2;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandles[textureNum];
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource[textureNum];

	for (int i = 0; i < textureNum; i++)
	{
		const DirectX::TexMetadata& metadata = mipImages[i].GetMetadata();

		textureResource[i] = CreateTextureResource(device, metadata);

		// metaDataを基にSRVの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = metadata.format;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
		srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

		UploadTextureData(textureResource[i], mipImages[i]);

		// CPU 側（ImGui分の1 + i）
		auto cpuHandle = GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSize, /*index=*/ 1 + i);
		// GPU 側も同様に
		auto gpuHandle = GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSize, /*index=*/ 1 + i);

		// SRV を生成
		device->CreateShaderResourceView(textureResource[i].Get(), &srvDesc, cpuHandle);
		textureSrvHandles[i] = gpuHandle;
	}

	std::vector<DirectX::ScratchImage> mipImagesMtl(modelData.size());
	for (size_t i = 0; i < modelData.size(); ++i)
	{
		mipImagesMtl[i] = LoadTexture(modelData[i].material.textureFilePath);
	}

	std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> textureSrvHandlesMtl(modelData.size());
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> textureResourceMtl(modelData.size());
	for (size_t i = 0; i < modelData.size(); ++i)
	{
		const DirectX::TexMetadata& metadata = mipImagesMtl[i].GetMetadata();

		textureResourceMtl[i] = CreateTextureResource(device, metadata);

		// metaDataを基にSRVの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = metadata.format;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
		srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

		UploadTextureData(textureResourceMtl[i], mipImagesMtl[i]);

		// CPU 側（ImGui分の1 + i）
		auto cpuHandle = GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSize, /*index=*/ static_cast<uint32_t>(1 + textureNum + i));
		// GPU 側も同様に
		auto gpuHandle = GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSize, /*index=*/ static_cast<uint32_t>(1 + textureNum + i));

		// SRV を生成
		device->CreateShaderResourceView(textureResourceMtl[i].Get(), &srvDesc, cpuHandle);
		textureSrvHandlesMtl[i] = gpuHandle;
	}

	Transform transformSprite{ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };

	int textureIndex = 0;
	const char* textureOptions[] = { "Checker", "monsterBall" };

	


	// ビューポート
	D3D12_VIEWPORT viewport{};
	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = static_cast<float>(kClientWidth);
	viewport.Height = static_cast<float>(kClientHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// シザー矩形
	D3D12_RECT scissorRect{};
	// 基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = kClientWidth;
	scissorRect.top = 0;
	scissorRect.bottom = kClientHeight;

	// ImGuiの初期化。詳細は省かれたが「こういうもん」らしい。
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX12_Init(device.Get(),
		swapChainDesc.BufferCount,
		rtvDesc.Format,
		srvDescriptorHeap.Get(),
		srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());


	SoundData soundData1 = SoundLoadWave("Resources/Alarm01.wav");

	Transform transform{ {1.0f, 1.0f, 1.0f}, {0.0f, pi, 0.0f}, {0.0f, 0.0f, 0.0f} };
	Transform transformSphere{ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {-3.0f, 0.0f, 0.0f} };
	Transform cameraTransform{ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -20.0f} };
	Matrix4x4 worldMatrix, cameraMatrix, viewMatrix, projectionMatrix, worldViewProjectionMatrix;
	Matrix4x4 worldMatrixSphere, wvpMatrixSphere;
	
	std::vector<std::array<float, 4>> materialColor;
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
	directionLightDirection.z = directionalLightData->direction.z;

	bool isDrawSprite = false;

	Transform uvTransformSprite
	{
		{1.0f, 1.0f, 1.0f},
		{0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f}
	};

	DebugCamera debugCamera;
	bool isDebugCamera = false;
	
	Vector2 mousePosition = { 0.0f, 0.0f };

	// ウィンドウの×ボタンが押されるまでループ
	while (msg.message != WM_QUIT)
	{
		// Windowにメッセージが来てたら最優先で処理させる
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			input->Update();

			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			// ゲームの処理

			//// 開発用ImGuiの処理。実際に開発用のUIを出す場合はここをゲーム固有の処理に置き換えること。
			//ImGui::ShowDemoWindow();

			ImGui::Begin("Window");

			ImGui::Checkbox("DrawSprite", &isDrawSprite);
			ImGui::Checkbox("DebugCamera", &isDebugCamera);

			ImGui::DragFloat2("UVScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
			ImGui::DragFloat2("UVTranslate", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
			ImGui::SliderAngle("UVRotate", &uvTransformSprite.rotate.z);

			ImGui::Combo("Texture", &textureIndex, textureOptions, IM_ARRAYSIZE(textureOptions));

			ImGui::DragFloat3("Scale 1", &transform.scale.x, 0.01f);
			ImGui::DragFloat3("Rotate 1", &transform.rotate.x, 1.0f / 180.0f * pi);
			ImGui::DragFloat3("Translate 1", &transform.translate.x, 0.01f);

			
			for (size_t i = 0; i < modelData.size(); ++i)
			{
				std::string label = "##Color" + std::to_string(i);
				if (ImGui::ColorEdit4(label.c_str(), materialColor[i].data()))
				{
					// materialColorを上書きする
					materialData[i]->color = XMFLOAT4(materialColor[i][0], materialColor[i][1], materialColor[i][2], materialColor[i][3]);
					// （＝GPU 側で使われるマテリアル色を更新）
				}
			}

			ImGui::DragFloat3("Scale 2", &transformSphere.scale.x, 0.01f);
			ImGui::DragFloat3("Rotate 2", &transformSphere.rotate.x, 1.0f / 180.0f * pi);
			ImGui::DragFloat3("Translate 2", &transformSphere.translate.x, 0.01f);


			ImGui::DragFloat3("DirectionalLightDirection", &directionLightDirection.x, 0.01f);


			ImGui::DragFloat("DirectionLightIntensity", &directionalLightData->intensity, 0.01f);

			if (ImGui::ColorEdit4("DirectionalLightColor", directionalLightColor))
			{
				// directionalLightColorを上書きする
				directionalLightData->color = XMFLOAT4(directionalLightColor[0], directionalLightColor[1], directionalLightColor[2], directionalLightColor[3]);
				// （＝GPU 側で使われるマテリアル色を更新）
			}

			ImGui::DragFloat3("CameraScale", &cameraTransform.scale.x, 0.01f);
			ImGui::DragFloat3("CameraRotate", &cameraTransform.rotate.x, 1.0f / 180.0f * pi);
			ImGui::DragFloat3("CameraTranslate", &cameraTransform.translate.x, 0.01f);

			ImGui::Text("MouseX: %.2f, MouseY: %.2f", mousePosition.x, mousePosition.y);

			ImGui::Text("Press 0 to play the sound");

			ImGui::End();

			directionLightDirection = Normalize(directionLightDirection);
			
			directionalLightData->direction = XMFLOAT3{ directionLightDirection.x, directionLightDirection.y, directionLightDirection.z };
			
			

			
			
			worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
			worldMatrixSphere = MakeAffineMatrix(transformSphere.scale, transformSphere.rotate, transformSphere.translate);
			cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);

			if (isDebugCamera)
			{
				debugCamera.Update(input, cameraTransform);
				viewMatrix = debugCamera.GetViewMatrix();
			}
			else
			{
				viewMatrix = Inverse(cameraMatrix);
			}
			
			projectionMatrix = MakePerspectiveFovMatrix(0.45f, static_cast<float>(kClientWidth) / static_cast<float>(kClientHeight), 0.1f, 100.0f);
			worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
			wvpMatrixSphere = Multiply(worldMatrixSphere, Multiply(viewMatrix, projectionMatrix));

			transformationMatrixData->World = worldMatrix;
			transformationMatrixData->WVP = worldViewProjectionMatrix;

			// SphereのTransformationMatrixを更新
			transformationMatrixDataSphere->World = worldMatrixSphere;
			transformationMatrixDataSphere->WVP = wvpMatrixSphere;

			// Sprite用のWorldViewProjectionMatrixを作る
			Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
			Matrix4x4 viewMatrixSprite = MakeIdentity4x4();
			Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(
				0.0f, 0.0f,float(kClientWidth), float(kClientHeight), 0.0f, 100.0f);
			Matrix4x4 worldViewProjectionMatrixSprite = Multiply(worldMatrixSprite, Multiply(viewMatrixSprite, projectionMatrixSprite));
			
			transformationMatrixDataSprite->World = worldMatrixSprite;
			transformationMatrixDataSprite->WVP = worldViewProjectionMatrixSprite;

			Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransformSprite.scale);
			uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransformSprite.rotate.z));
			uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransformSprite.translate));
			materialDataSprite->uvTransform = uvTransformMatrix;

			mousePosition.x = static_cast<float>(input->MousePoint(hwnd).x);
			mousePosition.y = static_cast<float>(input->MousePoint(hwnd).y);

			// ImGuiの内部コマンドを生成する
			ImGui::Render();

			// 描画の処理

			// これから書き込むバックバッファのインデックスを取得
			UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();

			// TransitionBarrierの設定
			D3D12_RESOURCE_BARRIER barrier{};
			// 今回のバリアはTransition
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			// Noneにしておく
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			// バリアを張る対象のリソース。現在のバックバッファに対して行う
			barrier.Transition.pResource = swapChainResources[backBufferIndex].Get();
			// 遷移前（現在）のResourceState
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			// 遷移後のResourceState
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			// TransitionBarrierを張る
			commandList->ResourceBarrier(1, &barrier);



			// 描画先のRTVとDSVを設定する
			D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, &dsvHandle);

			
			// 指定した深度で画面全体をクリアする
			commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);



			// 指定した色で画面全体をクリアする
			float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f };	// 青っぽい色、RGBAの順
			commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor, 0, nullptr);

			

			

			// 描画用のDescriptorHeapの設定
			Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeaps[] = { srvDescriptorHeap };
			commandList->SetDescriptorHeaps(1, descriptorHeaps->GetAddressOf());

			// 02_00
			commandList->RSSetViewports(1, &viewport); // Viewportを設定
			commandList->RSSetScissorRects(1, &scissorRect);	// Scissorを設定
			// RootSignatureを設定。PSOに設定しているけど別途設定が必要
			commandList->SetGraphicsRootSignature(rootSignature.Get());
			commandList->SetPipelineState(graphicsPipeLineState.Get());	// PS0を設定


			// 形状を設定。PS0に設定しているものとはまた別。同じものを設定すると考えておけば良い
			commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			//// SphereのIndexBufferViewを設定
			//commandList->IASetIndexBuffer(&indexBufferView);

			for (size_t i = 0; i < modelData.size(); ++i)
			{

				commandList->IASetVertexBuffers(0, 1, &vertexBufferView[i]);	// VBVを設定
				
				// マテリアルのCBufferの場所を設定
				commandList->SetGraphicsRootConstantBufferView(0, materialResource[i]->GetGPUVirtualAddress());

				// wvp用のCBufferの場所を設定
				commandList->SetGraphicsRootConstantBufferView(1, transformationResource->GetGPUVirtualAddress());

				// SRV用のDescriptorTableの先頭を設定。2はrootParameter[2]である。
				commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandlesMtl[i]);

				// 平行光源用のCBufferをバインド（rootParameter[3] = b1）
				commandList->SetGraphicsRootConstantBufferView(3, DirectionalLightResource->GetGPUVirtualAddress());

				// 描画！（DrawCall/ドローコール）。
				commandList->DrawInstanced(UINT(modelData[i].vertices.size()), 1, 0, 0);

				//// 描画！（DrawCall/ドローコール）。
				//commandList->DrawIndexedInstanced(kSubdivision * kSubdivision * 6, 1, 0, 0, 0);

			}
			
			// Sphereの描画
			commandList->IASetVertexBuffers(0, 1, &vertexBufferViewSphere);	// VBVを設定
			// SphereのIndexBufferViewを設定
			commandList->IASetIndexBuffer(&indexBufferViewSphere);
			// SphereのマテリアルのCBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(0, materialResourceSphere->GetGPUVirtualAddress());
			// SphereのTransformationMatrixCBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(1, transformationResourceSphere->GetGPUVirtualAddress());
			// SRV用のDescriptorTableの先頭を設定。2はrootParameter[2]である。
			commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandles[0]);	// SphereはCheckerを使う
			// 平行光源用のCBufferをバインド（rootParameter[3] = b1）
			commandList->SetGraphicsRootConstantBufferView(3, DirectionalLightResource->GetGPUVirtualAddress());
			// 描画！（DrawCall/ドローコール）。
			commandList->DrawIndexedInstanced(kSubdivision * kSubdivision * 6, 1, 0, 0, 0);
			

			// Spriteの描画。変更が必要なものだけ変更する。
			commandList->SetPipelineState(graphicsPipeLineStateSprite.Get());	// PS0を設定

			commandList->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);

			// SpriteのIndexBufferViewを設定
			commandList->IASetIndexBuffer(&indexBufferViewSprite);	

			// SpriteのマテリアルのCBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress());
			// TransformationMatrixCBufferの」場所を設定
			commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite->GetGPUVirtualAddress());
			// SRV用のDescriptorTableの先頭を設定。2はrootParameter[2]である。
			commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandles[textureIndex]);
			
			if (isDrawSprite)
			{
				// 描画！(DrawCall)
				commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
			}
			
			

			if (input->TriggerKey(DIK_0))
			{
				SoundPlayWave(xAudio2.Get(), soundData1);
			}


			// 実際のcommandListのImGuiの描画コマンドを積む
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());

			// 画面に描く処理はすべて終わり、画面に映すので、状態を遷移
			// 今回はRenderTargetからPresentにする
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
			// TransitionBarrierを張る
			commandList->ResourceBarrier(1, &barrier);


			// コマンドリストの内容を確定させる。すべてのコマンドを積んでからCloseすること
			hr = commandList->Close();
			assert(SUCCEEDED(hr));



			// GPUにコマンドリストの実行を行わせる
			Microsoft::WRL::ComPtr<ID3D12CommandList> commandLists[] = { commandList };
			commandQueue->ExecuteCommandLists(1, commandLists->GetAddressOf());
			// GPUとOSに画面の交換を行うよう通知する
			swapChain->Present(1, 0);

			// Fenceの値を更新
			fenceValue++;
			// GPUがここまでたどり着いたときに、Fenceの値に代入するようにSignalを送る
			commandQueue->Signal(fence.Get(), fenceValue);

			// Fenceの値が指定したSignal値にたどり着いているか確認する
			// GetCompleteValueの初期値はFence作成時に渡した初期値
			if (fence->GetCompletedValue() < fenceValue)
			{
				// 指定したSignalにたどりついていないので、たどり着くまで待つようにイベントを設定する
				fence->SetEventOnCompletion(fenceValue, fenceEvent);
				// イベント待つ
				WaitForSingleObject(fenceEvent, INFINITE);
			}

			// 次のフレーム用のコマンドリストを準備
			hr = commandAllocator->Reset();
			assert(SUCCEEDED(hr));
			hr = commandList->Reset(commandAllocator.Get(), nullptr);
			assert(SUCCEEDED(hr));

		}
	}

	// ImGuiの終了処理
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	// XAudio2解放
	xAudio2.Reset();

	// 音声データ解放
	SoundUnload(&soundData1);

	delete input;

	// オブジェクトの解放処理
	CloseHandle(fenceEvent);

	CloseWindow(hwnd);

	// ゲーム終了時にCOMの終了処理を行う
	CoUninitialize();
		
	


	return 0;
}



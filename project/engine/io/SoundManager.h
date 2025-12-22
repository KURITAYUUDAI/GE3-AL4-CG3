#pragma once
#include "DirectXBase.h"
#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")
#include <fstream>
#include <cassert>
#include <memory>
#include <unordered_map>

class SoundManager
{
public:

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

public:

	// シングルトンインスタンスの取得
	static SoundManager* GetInstance();
	// 終了
	void Finalize();

public: 

	// 初期化
	void Initialize();

public:

	void SoundLoadWave(const char* filename);

	void SoundUnload(const char* filename);

	void SoundPlayWave(const char* filename);

public:

	// 最大サウンドデータ数
	static const uint32_t kMaSoundCount;

private:	// シングルトンインスタンス

	// unique_ptr が delete するために使用する構造体
	struct Deleter
	{
		void operator()(SoundManager* p) const
		{
			// クラス内部のスコープなので private なデストラクタを呼べる
			delete p;
		}
	};

	// unique_ptr の型定義に Deleter を入れることでdeleteが可能になる
	static std::unique_ptr<SoundManager, Deleter> instance_;

	SoundManager() = default;
	~SoundManager() = default;
	SoundManager(SoundManager&) = delete;
	SoundManager& operator=(SoundManager&) = delete;

private:

	// サウンド再生エンジンをローカル変数で宣言
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;

	// 全てのボイスが再生時に必ず通る「マスターボイス」を宣言
	IXAudio2MasteringVoice* masterVoice_;

	// サウンドデータのコンテナ
	std::unordered_map<std::string, SoundData> soundDatas_;
};


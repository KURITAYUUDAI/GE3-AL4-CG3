#include "SoundManager.h"

std::unique_ptr<SoundManager, SoundManager::Deleter> SoundManager::instance_ = nullptr;

const uint32_t SoundManager::kMaSoundCount = 32;

SoundManager* SoundManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_.reset(new SoundManager());
	}
	return instance_.get();
}

void SoundManager::Finalize()
{

	// 音声データ解放
	for(auto it = soundDatas_.begin(); it != soundDatas_.end(); )
	{
		delete[] it->second.pBuffer;
		it->second.pBuffer = nullptr;

		it = soundDatas_.erase(it);
	}

	masterVoice_->DestroyVoice();

	// XAudio2解放
	xAudio2_.Reset();

	instance_.reset();
}

void SoundManager::Initialize()
{
	HRESULT hr;

	// インスタンスを生成
	hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr));

	// インスタンスを生成
	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(hr));

	soundDatas_.reserve(kMaSoundCount);
}

void SoundManager::SoundLoadWave(const char* filename)
{
	// 読み込み済みサウンドを検索
	if (soundDatas_.contains(filename))
	{
		return;
	}

	// サウンドデータ数上限チェック
	assert(soundDatas_.size() < kMaSoundCount);
	
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
	SoundData& soundData = soundDatas_[filename];

	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;
}

void SoundManager::SoundUnload(const char* filename)
{
	auto it = soundDatas_.find(filename);
	if (it == soundDatas_.end()) { return; }

	delete[] it->second.pBuffer;
	it->second.pBuffer = nullptr;


	// バッファのメモリを解放
	soundDatas_.erase(filename);
}

void SoundManager::SoundPlayWave(const char* filename)
{
	HRESULT result;

	// 波形フォーマットを基にSourceVoiceの生成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2_->CreateSourceVoice(&pSourceVoice, &soundDatas_[filename].wfex);
	assert(SUCCEEDED(result));

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundDatas_[filename].pBuffer;
	buf.AudioBytes = soundDatas_[filename].bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	// 波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->Start();
}

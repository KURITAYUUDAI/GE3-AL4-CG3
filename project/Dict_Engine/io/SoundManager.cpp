#include "SoundManager.h"
#include "StringUtility.h"

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
	for (auto it = soundDatas_.begin(); it != soundDatas_.end(); ) 
	{
		auto cur = it++;
		SoundUnload(cur->first.c_str());
	}

	masterVoice_->DestroyVoice();

	// XAudio2解放
	xAudio2_.Reset();

	instance_.reset();
}

void SoundManager::InitializeMF()
{
	HRESULT result;	// 他と使いまわし可能

	// Windows Media Foundation の初期化（ローカルファイル版）
	result = MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);
	assert(SUCCEEDED(result));
}

void SoundManager::FinalizeMF()
{
	HRESULT result;	// 他と使いまわし可能

	// Windows Media Foundation の終了
	result = MFShutdown();
	assert(SUCCEEDED(result));
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

void SoundManager::SoundLoadFile(const char* fullpath)
{
	// 読み込み済みサウンドを検索
	if (soundDatas_.contains(fullpath))
	{
		return;
	}

	// サウンドデータ数上限チェック
	assert(soundDatas_.size() < kMaSoundCount);
	
	// フルパスをワイド文字列に変換
	std::wstring filePathW = StringUtility::ConvertString(fullpath);
	HRESULT result;

	// SourceReader作成
	ComPtr<IMFSourceReader> pReader;
	result = MFCreateSourceReaderFromURL(filePathW.c_str(), nullptr, &pReader);
	assert(SUCCEEDED(result));

	// PCM形式にフォーマット指定する
	ComPtr<IMFMediaType> pPCMType;
	MFCreateMediaType(&pPCMType);
	pPCMType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	pPCMType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	result = pReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pPCMType.Get());
	assert(SUCCEEDED(result));

	// 実際にセットされたメディアタイプを取得する
	ComPtr<IMFMediaType> pOutType;
	pReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pOutType);

	// Waveフォーマットを取得する
	WAVEFORMATEX* waveFormat = nullptr;
	MFCreateWaveFormatExFromMFMediaType(pOutType.Get(), &waveFormat, nullptr);

	// コンテナに格納する音声データ
	SoundData& soundData = soundDatas_[fullpath];
	soundData.wfex = *waveFormat;

	// 生成したWaveフォーマットを解放
	CoTaskMemFree(waveFormat);

	// PCMデータのバッファを構築
	while (true)
	{
		ComPtr<IMFSample> pSample;
		DWORD streamIndex = 0, flags = 0;
		LONGLONG llTimeStamp = 0;
		// サンプルを読み込む
		result = pReader->ReadSample(
			MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, &streamIndex, &flags, &llTimeStamp, &pSample);
		// ストリームの末尾に達したら抜ける
		if(flags & MF_SOURCE_READERF_ENDOFSTREAM) break;

		if (pSample)
		{
			ComPtr<IMFMediaBuffer> pBuffer;
			// サンプルに含まれるサウンドデータのバッファーを一繋ぎにして取得
			pSample->ConvertToContiguousBuffer(&pBuffer);

			BYTE* pData = nullptr;	// データの読み取り用ポインタ
			DWORD maxLength = 0, currentLength = 0;
			// バッファ読み込み用にロック
			pBuffer->Lock(&pData, &maxLength, &currentLength);
			// バッファの末尾にデータを追加
			soundData.buffer.insert(soundData.buffer.end(), pData, pData + currentLength);
			pBuffer->Unlock();
		}
	}
}

void SoundManager::SoundUnload(const char* filename)
{
	auto it = soundDatas_.find(filename);
	if (it == soundDatas_.end()) { return; }

	it->second.buffer.clear();
	it->second.wfex = {};

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
	buf.pAudioData = soundDatas_[filename].buffer.data();
	buf.AudioBytes = static_cast<UINT32>(soundDatas_[filename].buffer.size());
	buf.Flags = XAUDIO2_END_OF_STREAM;

	// 波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->Start();
}

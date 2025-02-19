#pragma once

#include<xaudio2.h>
#include<fstream>
#include <wrl.h>

class AudioManager
{
public:
	/*ファイル構造体*/
	/*チャンクヘッダ*/
	struct ChunkHeader
	{
		char id[4];/*チャンク毎のID*/
		int32_t size;/*チャンクのサイズ*/
	};
	/*RIFFヘッダチャンク*/
	struct RiffHeader
	{
		ChunkHeader chunk;/*”RIFF"*/
		char type[4];/*"WAVE"*/
	};
	/*FMTチャンク*/
	struct FormatChunk
	{
		ChunkHeader chunk;/*"fmt"*/
		WAVEFORMATEX fmt;/*波形フォーマット*/
	};
	/*音声データ*/
	struct SoundData
	{
		WAVEFORMATEX wfex;     /*波形フォーマット*/
		BYTE* pBuffer;         /*バッファの先頭アドレス*/
		unsigned int bufferSize;/*バッファのサイズ*/
		IXAudio2SourceVoice* pSourceVoice; /* SourceVoice を保持 */
		float currentVolume = 1.0f;    /* 現在のボリューム */
		bool isPlaying = false; /* 再生中かどうか */
	};
	void Initialize();

	//void Update();

	SoundData SoundLordWave(const char* filename);

	/*音声データ解放*/
	void SoundUnLord(SoundData* soundData);

	/*音声再生*/
	void SoundPlayWave(SoundData& soundData, bool loop = false); // ループフラグを追加
	void SoundStop(SoundData& soundData); // Stop関数を追加
	void SoundStopFadeOut(SoundData& soundData, float duration);
	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	IXAudio2* GetXAudio2() { return xAudio2.Get(); }

	/// <summary>
	/// ボリュームを変更する
	/// </summary>
	/// <param name="soundData">音声データ</param>
	/// <param name="volume">設定するボリューム（0.0～1.0）</param>
	void SetVolume(SoundData& soundData, float volume);
private:
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice_ = nullptr;
};


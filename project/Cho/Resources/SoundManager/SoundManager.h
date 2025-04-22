#pragma once
class ResourceManager;
class SoundManager
{
public:
	// コンストラクタ
	SoundManager(ResourceManager* resourceManager):
		m_ResourceManager(resourceManager)
	{

	}
	// デストラクタ
	~SoundManager()
	{

	}
	//// 初期化
	//void Initialize();
	//// 終了処理
	//void Finalize();
	//// 更新
	//void Update();
	//// 解放
	//void Release();
	//// サウンドの読み込み
	//void LoadSound(const std::string& filePath);
	//// サウンドの再生
	//void PlaySound(const std::string& soundName);
	//// サウンドの停止
	//void StopSound(const std::string& soundName);
private:
	// リソースマネージャー
	ResourceManager* m_ResourceManager = nullptr;
};


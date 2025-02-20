#pragma once

/*--------------------------------------------
リソースマネージャクラス
--------------------------------------------*/
class ResourceManager
{
public:
	// コンストラクタ
	ResourceManager();
	// デストラクタ
	~ResourceManager();
	// 初期化
	void Initialize();
	// 終了処理
	void Finalize();
	// 更新
	void Update();
	// 解放
	void Release();
};


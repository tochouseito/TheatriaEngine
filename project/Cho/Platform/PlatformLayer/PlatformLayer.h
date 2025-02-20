#pragma once

/*--------------------------------------------
プラットフォーム独立層
--------------------------------------------*/
#include "Cho/Platform/Timer/Timer.h"

//C++
#include <memory>

class PlatformLayer
{
public:// メンバ関数

	// コンストラクタ
	PlatformLayer();

	// デストラクタ
	~PlatformLayer();

	// 初期化
	void Initialize();

	// 終了処理
	void Finalize();

	// 更新
	void Update();

	// 記録開始
	void StartFrame();

	// 記録終了
	void EndFrame();
private:// メンバ変数
	std::unique_ptr<Timer> timer = nullptr;
};


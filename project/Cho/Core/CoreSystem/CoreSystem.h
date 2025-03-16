#pragma once
/*--------------------------------------------
CoreSystemクラス
--------------------------------------------*/

#include "Core/ThreadManager/ThreadManager.h"

// C++
#include <memory>

class CoreSystem
{
public:// メンバ関数
	// コンストラクタ
	CoreSystem();

	// デストラクタ
	~CoreSystem();

	// 初期化
	void Initialize();

	// 終了処理
	void Finalize();

private:
};


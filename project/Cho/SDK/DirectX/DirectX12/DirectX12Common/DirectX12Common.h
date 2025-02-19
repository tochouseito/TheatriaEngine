#pragma once

#include<wrl.h>
/*--------------------------------------------
DirectX12共通クラス
--------------------------------------------*/

#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxgidebug.h>

class DirectX12Common
{
public:// メンバ関数

	// 初期化
	void Initialize();

	// 終了処理
	void Finalize();
};

/*--------------------------------------------
リソースリークチェッカー
--------------------------------------------*/
class ResourceLeakChecker
{
public:// メンバ関数

	// デストラクタ
	~ResourceLeakChecker();
};


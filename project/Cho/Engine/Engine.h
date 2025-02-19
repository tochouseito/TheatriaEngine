#pragma once
/*--------------------------------------------
エンジン基底クラス
--------------------------------------------*/

// C++
#include <memory>

class Engine
{
public:// method

	// デストラクタ
	virtual ~Engine() = default;

	virtual void Initialize() = 0;
	virtual void Finalize() = 0;
	virtual void Operation() = 0;
};


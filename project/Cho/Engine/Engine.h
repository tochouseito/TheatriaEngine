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

	virtual void Initialize() {}
	virtual void Finalize() {}
	virtual void Operation() {}
};


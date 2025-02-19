#pragma once
/*--------------------------------------------
エンジンクラス
--------------------------------------------*/

// C++
#include <memory>

class Engine
{
public:// method
	virtual void Initialize() = 0;
	virtual void Finalize() = 0;
	virtual void Operation() = 0;
};


#pragma once

/*--------------------------------------------
ChoEngineクラス
--------------------------------------------*/

#include "Cho/Engine/Engine.h"
#include "Cho/OS/Windows/WinApp/WinApp.h"

class ChoEngine : public Engine
{
public:// method

	// 初期化
	void Initialize() override;

	// 終了処理
	void Finalize() override;

	// 稼働
	void Operation() override;
private:// member

	// Windowsアプリケーション
	std::unique_ptr<WinApp> winApp = nullptr;
};


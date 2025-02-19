#pragma once

/*--------------------------------------------
ChoEngineクラス
--------------------------------------------*/

#include "Cho/Engine/Engine.h"

// Windows
#include "Cho/OS/Windows/WinApp/WinApp.h"

// DirectX12
#include "Cho/SDK/DirectX/DirectX12/DirectX12Common/DirectX12Common.h"

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

	// DirectX12
	std::unique_ptr<ResourceLeakChecker> resourceLeakChecker = nullptr;// ResourceLeakChecker
	std::unique_ptr<DirectX12Common> dx12 = nullptr;// DirectX12Common
};


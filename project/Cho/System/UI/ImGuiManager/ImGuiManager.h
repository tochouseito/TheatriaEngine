#pragma once

#include<d3d12.h>
#include <imgui.h>
#include <wrl.h>

class WinApp;
class D3DDevice;
class D3DCommand;
class ResourceViewManager;
class ImGuiManager
{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(WinApp* win,D3DDevice* d3dDevice, D3DCommand* d3dCommand,ResourceViewManager* RVManager);

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

	/// <summary>
	/// ImGui受付開始
	/// </summary>
	void Begin();

	/// <summary>
	/// ImGui受付終了
	/// </summary>
	void End();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:
	D3DCommand* d3dCommand_ = nullptr;
};


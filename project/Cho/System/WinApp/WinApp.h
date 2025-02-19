#pragma once
/*-------------------------------------------------------------------------------------------*/
/*                                                                                           */
/*                                      ウィンドウズアプリケーションクラス					　　　*/
/*                                                                                           */
/*-------------------------------------------------------------------------------------------*/

#include <Windows.h>
//#include <ole.h>
#include <ole2.h>
#include <timeapi.h>
#include<cstdint>

class WinApp
{
public:// 静的メンバ関数

	/// <summary>
	/// ウィンドウプロシージャ
	/// </summary>
	/// <returns></returns>
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg,
		WPARAM wparam, LPARAM lparam);

public:// メンバ関数

	/// <summary>
	/// デストラクタ
	/// </summary>
	~WinApp() = default;

	/// <summary>
	/// ゲームウィンドウの作成
	/// </summary>
	void CreateGameWindow();

	/// <summary>
	/// メッセージ処理
	/// </summary>
	bool ProcessMessage();

	bool IsEndApp();

	/// <summary>
	/// ウィンドウの破棄
	/// </summary>
	void TerminateWindow();

	HWND GetHwnd() const {
		return hwnd_;
	}

	HINSTANCE GetHInstance() const { return wc_.hInstance; }

private:
	// ウィンドウサイズ変更時の処理
	static void OnWindowResize(int width, int height);

private:// メンバ変数
	static HWND hwnd_;

	WNDCLASS wc_{}; // ウィンドウクラス

	static bool isAppRunning; // アプリケーションが動作中かを示すフラグ
};


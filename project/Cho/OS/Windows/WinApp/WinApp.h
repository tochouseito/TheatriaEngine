#pragma once

/*--------------------------------------------
WinAppクラス
--------------------------------------------*/

#include <Windows.h>
#include <ole2.h>
#include <timeapi.h>
#include<cstdint>

class WinApp
{
public:// 静的メンバ関数

	// ウィンドウプロシージャ
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:// メンバ関数

	// デストラクタ
	~WinApp() = default;

	// ゲームウィンドウの作成
	void CreateGameWindow();

	// メッセージ処理
	bool ProcessMessage();

	bool IsEndApp();

	// ウィンドウのタイトルバー変更
	void SetWindowTitle(const wchar_t* title);

	// ウィンドウの破棄
	void TerminateWindow();

	HWND GetHwnd() const { return hwnd_; }

	HINSTANCE GetHInstance() const { return wc_.hInstance; }

private:

	// ウィンドウサイズ変更時の処理
	static void OnWindowResize(int width, int height);

private:// メンバ変数
	static HWND hwnd_;

	WNDCLASS wc_{}; // ウィンドウクラス

	static bool isAppRunning; // アプリケーションが動作中かを示すフラグ
};


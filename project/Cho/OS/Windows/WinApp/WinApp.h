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
	static void CreateGameWindow();

	// メッセージ処理
	static bool ProcessMessage();

	static bool IsEndApp();

	// ウィンドウのタイトルバー変更
	static void SetWindowTitle(const wchar_t* title);

	// ウィンドウの破棄
	static void TerminateWindow();

	static HWND GetHwnd() { return hwnd; }

	static HINSTANCE GetHInstance() { return wc.hInstance; }

private:

	// ウィンドウサイズ変更時の処理
	static void OnWindowResize(int width, int height);

private:// メンバ変数
	static HWND hwnd;
	static WNDCLASS wc; // ウィンドウクラス
	static bool isAppRunning; // アプリケーションが動作中かを示すフラグ
	static int32_t windowWidth; // ウィンドウの幅
	static int32_t windowHeight; // ウィンドウの高さ
};


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

	/*Getter*/
	static HWND GetHWND() { return m_HWND; }
	static HINSTANCE GetHInstance() { return m_WC.hInstance; }
	static int32_t GetWindowWidth() { return m_WindowWidth; }
	static int32_t GetWindowHeight() { return m_WindowHeight; }
private:

	// ウィンドウサイズ変更時の処理
	static void OnWindowResize(int width, int height);

private:// メンバ変数
	static HWND m_HWND; // ウィンドウハンドル
	static WNDCLASS m_WC; // ウィンドウクラス
	static bool m_IsRun; // アプリケーションが動作中かを示すフラグ
	static int32_t m_WindowWidth; // ウィンドウの幅
	static int32_t m_WindowHeight; // ウィンドウの高さ
};


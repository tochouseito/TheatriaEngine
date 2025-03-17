#include "pch.h"
#include "WinApp.h"
#pragma comment(lib,"winmm.lib")
#include <shellapi.h>
#include "Core/Log/Log.h"
#include <imgui.h>
#include <imgui_impl_win32.h>
extern IMGUI_IMPL_API LRESULT
ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

HWND WinApp::m_HWND = nullptr;
WNDCLASS WinApp::m_WC = {}; // ウィンドウクラス
bool WinApp::m_IsRun=true; // アプリケーションが動作中かを示すフラグ
int32_t WinApp::m_WindowWidth = 1280; // ウィンドウの幅
int32_t WinApp::m_WindowHeight = 720; // ウィンドウの高さ

// ウィンドウプロシージャ
LRESULT CALLBACK WinApp::WindowProc(HWND hWnd, UINT msg,
	WPARAM wparam, LPARAM lparam) {
//#ifdef _DEBUG
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wparam, lparam)) {
		return true;
	}
//#endif
	// メッセージに応じてゲーム固有の処理を行う
	switch (msg) {
	case WM_GETMINMAXINFO: {
		MINMAXINFO* pMinMaxInfo = reinterpret_cast<MINMAXINFO*>(lparam);
		pMinMaxInfo->ptMinTrackSize.x = 800; // 最小幅を設定（例：800）
		pMinMaxInfo->ptMinTrackSize.y = 600; // 最小高さを設定（例：600）
		break;
	}

	case WM_SIZE:
		if (wparam != SIZE_MINIMIZED) {
			int width = LOWORD(lparam);
			int height = HIWORD(lparam);
			OnWindowResize(width, height);
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		m_IsRun = false;
		return 0;
	}
	// 標準のメッセージ処理を行う
	return DefWindowProc(hWnd, msg, wparam, lparam);
}

// ゲームウィンドウの作成
void WinApp::CreateGameWindow() {
	// Log出力
	ChoLog("CreateGameWindow");

	HRESULT hr;

	// COM初期化
	hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	// ウィンドウプロシージャ
	m_WC.lpfnWndProc = WindowProc;
	// ウィンドウクラス名
	m_WC.lpszClassName = L"ChoWindowClass";
	// インスタンスハンドル
	m_WC.hInstance = GetModuleHandle(nullptr);
	// カーソル
	m_WC.hCursor = LoadCursor(nullptr, IDC_ARROW);

	// ウィンドウクラスを登録する
	RegisterClass(&m_WC);

	// ウィンドウサイズを表す構造体にクライアント領域を入れる
	RECT wrc = { 0,0,m_WindowWidth,m_WindowHeight};

	// クライアント領域を元に実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// ウィンドウの生成
	m_HWND = CreateWindow(
		m_WC.lpszClassName,		// 利用するクラス名
		L"Cho",					// タイトルバーの文字
		WS_OVERLAPPEDWINDOW,	// よく見るウィンドウスタイル
		CW_USEDEFAULT,			// 表示X座標
		CW_USEDEFAULT,			// 表示Y座標
		wrc.right - wrc.left,	// ウィンドウ横幅
		wrc.bottom - wrc.top,	// ウィンドウ縦幅
		nullptr,				// 親ウィンドウハンドル
		nullptr,				// メニューハンドル
		m_WC.hInstance,			// インスタンスハンドル
		nullptr);				// オプション

	// システムタイマーの分解能を上げる
	timeBeginPeriod(1);

	// ウィンドウを表示する
	ShowWindow(m_HWND, SW_SHOW);

	// Log出力
	ChoLog("CreateGameWindow End");
}

/// <summary>
/// メッセージ処理
/// </summary>
bool WinApp::ProcessMessage() {
	MSG msg{};
	// Windowにメッセージが来てたら最優先で処理させる
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (msg.message == WM_QUIT) {
		return true;
	}
	return false;
}

bool WinApp::IsEndApp()
{
	if (!m_IsRun) {
		return true;
	}
	return false;
}

void WinApp::SetWindowTitle(const wchar_t* title)
{
	SetWindowText(m_HWND, title);
}

void WinApp::TerminateWindow()
{
	// Log出力
	ChoLog("TerminateWindow");

	// ウィンドウを完全に閉じる
	DestroyWindow(m_HWND);

	// ウィンドウクラスを登録解除
	UnregisterClass(m_WC.lpszClassName, m_WC.hInstance);

	// COM 終了
	CoUninitialize();

	// Log出力
	ChoLog("TerminateWindow End");
}

void WinApp::OpenWebURL(const wchar_t* url)
{
	ShellExecuteW(nullptr, L"open", url, nullptr, nullptr, SW_SHOWNORMAL);
}

// ウィンドウサイズ変更時の処理
void WinApp::OnWindowResize(int width, int height) {
	if (width != 0 && height != 0) {
		m_WindowWidth = width;
		m_WindowHeight = height;
		// 新しいクライアント領域のサイズを hwnd_ から取得
		RECT rect;
		GetClientRect(m_HWND, &rect);
		int newWidth = rect.right - rect.left;
		int newHeight = rect.bottom - rect.top;
		newWidth;
		newHeight;
		static bool flag = false;
		if (flag) {
			// ImGuiのディスプレイサイズを更新
			ImGuiIO& io = ImGui::GetIO();
			io.DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));

		}
		flag = true;
	}
}

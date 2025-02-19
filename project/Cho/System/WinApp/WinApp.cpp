#include "PrecompiledHeader.h"
#include "WinApp.h"
#pragma comment(lib,"winmm.lib")
//#ifdef _DEBUG
#include <imgui_impl_win32.h>
extern IMGUI_IMPL_API LRESULT
ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
//#endif

#include"imgui.h"

HWND WinApp::hwnd_ = nullptr;
bool WinApp::isAppRunning = true;

// ウィンドウプロシージャ
LRESULT CALLBACK WinApp::WindowProc(HWND hwnd, UINT msg,
	WPARAM wparam, LPARAM lparam) {
//#ifdef _DEBUG
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
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
		isAppRunning = false;
		return 0;
	}
	// 標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

// ゲームウィンドウの作成
void WinApp::CreateGameWindow() {

	HRESULT hr;

	// COM初期化
	hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	// ウィンドウプロシージャ
	wc_.lpfnWndProc = WindowProc;
	// ウィンドウクラス名
	wc_.lpszClassName = L"ChoWindowClass";
	// インスタンスハンドル
	wc_.hInstance = GetModuleHandle(nullptr);
	// カーソル
	wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);

	// ウィンドウクラスを登録する
	RegisterClass(&wc_);

	// ウィンドウサイズを表す構造体にクライアント領域を入れる
	RECT wrc = { 0,0,WindowWidth(),WindowHeight()};

	// クライアント領域を元に実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// ウィンドウの生成
	hwnd_ = CreateWindow(
		wc_.lpszClassName,		// 利用するクラス名
		L"Cho",					// タイトルバーの文字
		WS_OVERLAPPEDWINDOW,	// よく見るウィンドウスタイル
		CW_USEDEFAULT,			// 表示X座標
		CW_USEDEFAULT,			// 表示Y座標
		wrc.right - wrc.left,	// ウィンドウ横幅
		wrc.bottom - wrc.top,	// ウィンドウ縦幅
		nullptr,				// 親ウィンドウハンドル
		nullptr,				// メニューハンドル
		wc_.hInstance,			// インスタンスハンドル
		nullptr);				// オプション

	// システムタイマーの分解能を上げる
	timeBeginPeriod(1);

	// ウィンドウを表示する
	ShowWindow(hwnd_, SW_SHOW);
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
	if (!isAppRunning) {
		return true;
	}
	return false;
}

void WinApp::TerminateWindow()
{
	// ウィンドウを完全に閉じる
	DestroyWindow(hwnd_);

	// ウィンドウクラスを登録解除
	UnregisterClass(wc_.lpszClassName, wc_.hInstance);

	// COM 終了
	CoUninitialize();
}

// ウィンドウサイズ変更時の処理
void WinApp::OnWindowResize(int width, int height) {
	if (width != 0 && height != 0) {
		SystemState::GetInstance().SetWindowWidth(width);
		SystemState::GetInstance().SetWindowHeight(height);
		// 新しいクライアント領域のサイズを hwnd_ から取得
		RECT rect;
		GetClientRect(hwnd_, &rect);
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

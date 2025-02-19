#pragma once

#include "Vector2.h"
#include <Windows.h>
#include <array>
#include <vector>
#include <wrl.h>

#include<Xinput.h>
#define DIRECTINPUT_VERSION 0x0800 // DirectInputのバージョン指定
#include<dinput.h>

enum class PadButton {
	A = XINPUT_GAMEPAD_A,
	B = XINPUT_GAMEPAD_B,
	X = XINPUT_GAMEPAD_X,
	Y = XINPUT_GAMEPAD_Y,
	Up = XINPUT_GAMEPAD_DPAD_UP,
	Down = XINPUT_GAMEPAD_DPAD_DOWN,
	Left = XINPUT_GAMEPAD_DPAD_LEFT,
	Right = XINPUT_GAMEPAD_DPAD_RIGHT,
	Start = XINPUT_GAMEPAD_START,
	Back = XINPUT_GAMEPAD_BACK,
	LThumb = XINPUT_GAMEPAD_LEFT_THUMB,
	RThumb = XINPUT_GAMEPAD_RIGHT_THUMB,
	LShoulder = XINPUT_GAMEPAD_LEFT_SHOULDER,
	RShoulder = XINPUT_GAMEPAD_RIGHT_SHOULDER,
	LTrigger = VK_PAD_LTRIGGER,
	RTrigger = VK_PAD_RTRIGGER,
};

enum MouseButton {
	Left = 0,
	Right,
	Center,
	Extra1,
	Extra2,
};

enum class LR {
	LEFT = 0,
	RIGHT
};

class WinApp;

class InputManager
{
public:

	struct MouseMove {
		LONG lX;
		LONG lY;
		LONG lZ;
	};
	enum class PadType {
		DirectInput,
		XInput,
	};
	// variantがC++17から
	union State {
		XINPUT_STATE xInput_;
		DIJOYSTATE2 directInput_;
	};
	struct Joystick {
		Microsoft::WRL::ComPtr<IDirectInputDevice8> device_;
		int32_t deadZoneL_;
		int32_t deadZoneR_;
		PadType type_;
		State state_;
		State statePre_;
	};

	void Initialize(WinApp* winApp);

	void Update();

	/// <summary>
	/// キーの押下をチェック
	/// </summary>
	/// <param name="keyNumber">キー番号( DIK_0 等)</param>
	/// <returns>押されているか</returns>
	//bool PushKey(BYTE keyNumber) const;
	bool PushKey(uint8_t keyNumber) const;

	/// <summary>
	/// キーのトリガーをチェック
	/// </summary>
	/// <param name="keyNumber">キー番号( DIK_0 等)</param>
	/// <returns>トリガーか</returns>
	//bool TriggerKey(BYTE keyNumber) const;
	bool TriggerKey(uint8_t keyNumber) const;

	/// <summary>
	/// 全マウス情報取得
	/// </summary>
	/// <returns>マウス情報</returns>
	const DIMOUSESTATE2& GetAllMouse() const;

	/// <summary>
	/// マウスの押下をチェック
	/// </summary>
	/// <param name="buttonNumber">マウスボタン番号(0:左,1:右,2:中,3~7:拡張マウスボタン)</param>
	/// <returns>押されているか</returns>
	bool IsPressMouse(int32_t mouseNumber) const;

	/// <summary>
	/// マウスのトリガーをチェック。押した瞬間だけtrueになる
	/// </summary>
	/// <param name="buttonNumber">マウスボタン番号(0:左,1:右,2:中,3~7:拡張マウスボタン)</param>
	/// <returns>トリガーか</returns>
	bool IsTriggerMouse(int32_t buttonNumber) const;

	/// <summary>
	/// マウス移動量を取得
	/// </summary>
	/// <returns>マウス移動量</returns>
	MouseMove GetMouseMove();

	/// <summary>
	/// ホイールスクロール量を取得する
	/// </summary>
	/// <returns>ホイールスクロール量。奥側に回したら+。Windowsの設定で逆にしてたら逆</returns>
	int32_t GetWheel() const;

	/// <summary>
	/// マウスの位置を取得する（ウィンドウ座標系）
	/// </summary>
	/// <returns>マウスの位置</returns>
	const Vector2& GetMouseWindowPosition() const;

	/// <summary>
	/// マウスの位置を取得する（スクリーン座標系）
	/// </summary>
	/// <returns>マウスの位置</returns>
	Vector2 GetMouseScreenPosition() const;

	/// <summary>
	/// 現在のジョイスティック状態を取得する
	/// </summary>
	/// <param name="stickNo">ジョイスティック番号</param>
	/// <param name="out">現在のジョイスティック状態</param>
	/// <returns>正しく取得できたか</returns>
	bool GetJoystickState(int32_t stickNo, XINPUT_STATE& out) const;

	/// <summary>
	/// 前回のジョイスティック状態を取得する
	/// </summary>
	/// <param name="stickNo">ジョイスティック番号</param>
	/// <param name="out">前回のジョイスティック状態</param>
	/// <returns>正しく取得できたか</returns>
	bool GetJoystickStatePrevious(int32_t stickNo, XINPUT_STATE& out) const;

	/// <summary>
	/// デッドゾーンを設定する
	/// </summary>
	/// <param name="stickNo">ジョイスティック番号</param>
	/// <param name="deadZoneL">デッドゾーン左スティック 0~32768</param>
	/// <param name="deadZoneR">デッドゾーン右スティック 0~32768</param>
	/// <returns>正しく取得できたか</returns>
	void SetJoystickDeadZone(int32_t stickNo, int32_t deadZoneL, int32_t deadZoneR);

	/// <summary>
	/// 接続されているジョイスティック数を取得する
	/// </summary>
	/// <returns>接続されているジョイスティック数</returns>
	size_t GetNumberOfJoysticks();

	// パッドの押されているボタン、スティックの値を取得
	bool IsTriggerPadButton(PadButton button, int32_t stickNo = 0);
	bool IsPressPadButton(PadButton button, int32_t stickNo = 0);
	Vector2 GetStickValue(LR padStick, int32_t stickNo = 0);
	float GetLRTrigger(LR LorR, int32_t stickNo = 0);

	Vector2 CheckAndWarpMouse();


private:
		bool IsTriggerTrigger(LR LorR, int32_t stickNo = 0);
public:
	Microsoft::WRL::ComPtr<IDirectInput8> directInput = nullptr;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard = nullptr;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> devMouse = nullptr;
	std::vector<Joystick> devJoysticks_;
	BYTE key[256] = {};
	BYTE preKey[256] = {};
	DIMOUSESTATE2 mouse = {};
	DIMOUSESTATE2 mousePre = {};
	HWND hwnd_ = {};
	Vector2 mousePosition_;

	WinApp* winApp_ = nullptr;
};


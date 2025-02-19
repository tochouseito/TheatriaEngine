#include "PrecompiledHeader.h"
#include "InputManager.h"
#include"WinApp/WinApp.h"
#include<assert.h>

void InputManager::Initialize(WinApp* winApp)
{
	winApp_ = winApp;
	HRESULT result;

	// DirectInoutの初期化
	result = DirectInput8Create(
		winApp_->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	// キーボードデバイスの生成
	//IDirectInputDevice8* keyboard = nullptr;
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	// 入力データの形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);// 標準形式
	assert(SUCCEEDED(result));

	// 排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(
		winApp_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

	/*マウスデバイスの生成*/
	result = directInput->CreateDevice(GUID_SysMouse, &devMouse, NULL);
	assert(SUCCEEDED(result));

	/*入力データの形式のセット*/
	result = devMouse->SetDataFormat(&c_dfDIMouse2);/*標準形式*/
	assert(SUCCEEDED(result));

	/*排他制御レベルのセット*/
	result = devMouse->SetCooperativeLevel(
		winApp_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

	// XInputパッドの準備（4つのパッドに対応）
	for (int i = 0; i < 4; ++i) {
		Joystick joystick = {};
		joystick.type_ = PadType::XInput;
		devJoysticks_.push_back(joystick);
	}
}

void InputManager::Update()
{
	// 前回のキー入力を保存
	memcpy(preKey, key, sizeof(key));
	/*マウス*/
	mousePre = mouse;

	// 現在のウィンドウがフォーカスされているかを確認
	if (GetForegroundWindow() == winApp_->GetHwnd())
	{

		// キーボード情報の取得開始
		keyboard->Acquire();
		/*マウス*/
		devMouse->Acquire();

		// 全キーの入力状態を取得する
		keyboard->GetDeviceState(sizeof(key), key);
		devMouse->GetDeviceState(sizeof(mouse), &mouse);

		// マウス位置の取得
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(winApp_->GetHwnd(), &point);
		mousePosition_ = Vector2(static_cast<float>(point.x), static_cast<float>(point.y));

		// XInputコントローラーの入力取得
		for (int i = 0; i < 4; ++i) {
			Joystick& joystick = devJoysticks_[i];

			if (joystick.type_ == PadType::XInput) {
				joystick.statePre_ = joystick.state_; // 前回の状態を保存

				// XInputのステートを取得
				if (XInputGetState(i, &joystick.state_.xInput_) == ERROR_SUCCESS) {
					// XInputのコントローラーが接続されている場合の処理

					// 左スティック
					float leftThumbX = joystick.state_.xInput_.Gamepad.sThumbLX;
					float leftThumbY = joystick.state_.xInput_.Gamepad.sThumbLY;

					// 左スティックのデッドゾーン処理
					if (std::abs(leftThumbX) < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
						//leftThumbX = 0;
						joystick.state_.xInput_.Gamepad.sThumbLX = 0;
					}
					if (std::abs(leftThumbY) < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
						//leftThumbY = 0;
						joystick.state_.xInput_.Gamepad.sThumbLY = 0;
					}

					// 右スティック
					float rightThumbX = joystick.state_.xInput_.Gamepad.sThumbRX;
					float rightThumbY = joystick.state_.xInput_.Gamepad.sThumbRY;

					// 右スティックのデッドゾーン処理
					if (std::abs(rightThumbX) < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
						//rightThumbX = 0;
						joystick.state_.xInput_.Gamepad.sThumbRX = 0;
					}
					if (std::abs(rightThumbY) < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
						//rightThumbY = 0;
						joystick.state_.xInput_.Gamepad.sThumbRY = 0;
					}

					// トリガー
					BYTE leftTrigger = joystick.state_.xInput_.Gamepad.bLeftTrigger;
					BYTE rightTrigger = joystick.state_.xInput_.Gamepad.bRightTrigger;

					// トリガーのデッドゾーン処理
					if (leftTrigger < XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {
						//leftTrigger = 0;
						joystick.state_.xInput_.Gamepad.bLeftTrigger = 0;
					}
					if (rightTrigger < XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {
						//rightTrigger = 0;
						joystick.state_.xInput_.Gamepad.bRightTrigger = 0;
					}

				}
			}
		}
	}
	else
	{
		// フォーカスが外れている場合はキーとマウスの状態をリセット
		memset(key, 0, sizeof(key));
		memset(&mouse, 0, sizeof(mouse));
	}
}

bool InputManager::PushKey(uint8_t keyNumber) const
{
	if (key[keyNumber]) {
		return true;
	}
	return false;
}

bool InputManager::TriggerKey(uint8_t keyNumber) const
{
	if (key[keyNumber] && !preKey[keyNumber]) {
		return true;
	}
	return false;
}

const DIMOUSESTATE2& InputManager::GetAllMouse() const
{
	return mouse;
}

bool InputManager::IsPressMouse(int32_t mouseNumber) const
{
	return mouse.rgbButtons[mouseNumber] & 0x80;
}

bool InputManager::IsTriggerMouse(int32_t buttonNumber) const
{
	return (mouse.rgbButtons[buttonNumber] & 0x80) && !(mousePre.rgbButtons[buttonNumber] & 0x80);
}

InputManager::MouseMove InputManager::GetMouseMove()
{
	MouseMove move = { mouse.lX, mouse.lY, mouse.lZ };
	return move;
}

int32_t InputManager::GetWheel() const
{
	return mouse.lZ;
}

const Vector2& InputManager::GetMouseWindowPosition() const
{
	return mousePosition_;
}

Vector2 InputManager::GetMouseScreenPosition() const
{
	// マウス位置の取得
	POINT point;
	GetCursorPos(&point);
	Vector2 result=Vector2(static_cast<float>(point.x), static_cast<float>(point.y));
	return result;
}

bool InputManager::GetJoystickState(int32_t stickNo, XINPUT_STATE& out) const
{
	if (stickNo < 0 || stickNo >= devJoysticks_.size()) return false;
	if (devJoysticks_[stickNo].type_ != PadType::XInput) return false;

	out = devJoysticks_[stickNo].state_.xInput_;
	return true;
}

bool InputManager::GetJoystickStatePrevious(int32_t stickNo, XINPUT_STATE& out) const
{
	if (stickNo < 0 || stickNo >= devJoysticks_.size()) return false;
	if (devJoysticks_[stickNo].type_ != PadType::XInput) return false;

	out = devJoysticks_[stickNo].statePre_.xInput_;
	return true;
}

void InputManager::SetJoystickDeadZone(int32_t stickNo, int32_t deadZoneL, int32_t deadZoneR)
{
	stickNo;
	deadZoneL;
	deadZoneR;
}

size_t InputManager::GetNumberOfJoysticks()
{
	return size_t();
}

bool InputManager::IsTriggerPadButton(PadButton button, int32_t stickNo)
{
	XINPUT_STATE currentState;
	XINPUT_STATE previousState;

	if (button == PadButton::LTrigger or button == PadButton::RTrigger) {
		return IsTriggerTrigger(button == PadButton::LTrigger ? LR::LEFT : LR::RIGHT, stickNo);

	}
	else {
		if (GetJoystickState(stickNo, currentState) && GetJoystickStatePrevious(stickNo, previousState)) {
			return (
				(currentState.Gamepad.wButtons & (WORD)button) != 0) &&
				((previousState.Gamepad.wButtons & (WORD)button) == 0
					);
		}
	}

	return false;
}

bool InputManager::IsPressPadButton(PadButton button, int32_t stickNo)
{
	XINPUT_STATE currentState;

	if (button == PadButton::LTrigger or button == PadButton::RTrigger) {
		return GetLRTrigger(button == PadButton::LTrigger ? LR::LEFT : LR::RIGHT, stickNo) > 0.8f;

	}
	else {
		if (GetJoystickState(stickNo, currentState)) {
			// 押されている場合は true を返す
			return (currentState.Gamepad.wButtons & (WORD)button) != 0;
		}
	}
	return false;
}

Vector2 InputManager::GetStickValue(LR padStick, int32_t stickNo)
{
	Vector2 result{ 0.0f,0.0f };

	XINPUT_STATE currentState;
	if (GetJoystickState(stickNo, currentState)) {
		switch (padStick) {
		case LR::LEFT:
			result = Vector2(
				static_cast<float>(currentState.Gamepad.sThumbLX) / 32768.0f,
				static_cast<float>(currentState.Gamepad.sThumbLY) / 32768.0f
			);
			break;
		case LR::RIGHT:
			result = Vector2(
				static_cast<float>(currentState.Gamepad.sThumbRX) / 32768.0f,
				static_cast<float>(currentState.Gamepad.sThumbRY) / 32768.0f
			);
			break;
		}
	}

	return result;
}

float InputManager::GetLRTrigger(LR LorR, int32_t stickNo)
{
	XINPUT_STATE currentState;
	float value = 0;

	if (GetJoystickState(stickNo, currentState)) {
		if (LorR == LR::LEFT) {
			value = static_cast<float>(currentState.Gamepad.bLeftTrigger) / 255.0f;
		}
		else {
			value = static_cast<float>(currentState.Gamepad.bRightTrigger) / 255.0f;
		}
	}

	return value;
}

Vector2 InputManager::CheckAndWarpMouse()
{
	if (IsPressMouse(Left) ||
		IsPressMouse(Right) ||
		IsPressMouse(Center)) {

		// 画面の解像度を取得
		RECT screenRect;
		GetClientRect(GetDesktopWindow(), &screenRect);
		int screenWidth = screenRect.right;
		int screenHeight = screenRect.bottom;

		// マウスカーソルの現在位置を取得
		POINT cursorPos;
		GetCursorPos(&cursorPos);

		// マウスの相対移動量（補正用）
		float deltaX = 0.0f;
		float deltaY = 0.0f;

		// 画面端でワープ処理
		if (cursorPos.x <= 0) {
			deltaX = static_cast<float>(screenWidth - 2); // ワープによる移動量
			SetCursorPos(screenWidth - 2, cursorPos.y);
		}
		else if (cursorPos.x >= screenWidth - 1) {
			deltaX = static_cast<float>(-screenWidth + 2);
			SetCursorPos(1, cursorPos.y);
		}

		if (cursorPos.y <= 0) {
			deltaY = static_cast<float>(screenHeight - 2);
			SetCursorPos(cursorPos.x, screenHeight - 2);
		}
		else if (cursorPos.y >= screenHeight - 1) {
			deltaY = static_cast<float>(-screenHeight + 2);
			SetCursorPos(cursorPos.x, 1);
		}

		// マウスの相対移動量を補正
		Vector2 result = { deltaX,deltaY };
		return result;
	}
	else {
		return { 0.0f,0.0f };
	}

}

bool InputManager::IsTriggerTrigger(LR LorR, int32_t stickNo)
{
	XINPUT_STATE currentState;
	XINPUT_STATE previousState;
	float value[2] = { 0.0f };

	if (GetJoystickState(stickNo, currentState)) {
		if (LorR == LR::LEFT) {
			value[0] = static_cast<float>(currentState.Gamepad.bLeftTrigger) / 255.0f;
		}
		else {
			value[0] = static_cast<float>(currentState.Gamepad.bRightTrigger) / 255.0f;
		}
	}

	if (GetJoystickStatePrevious(stickNo, previousState)) {
		if (LorR == LR::LEFT) {
			value[1] = static_cast<float>(previousState.Gamepad.bLeftTrigger) / 255.0f;
		}
		else {
			value[1] = static_cast<float>(previousState.Gamepad.bRightTrigger) / 255.0f;
		}
	}

	return value[0] > 0.8f && value[1] <= 0.8f;
}

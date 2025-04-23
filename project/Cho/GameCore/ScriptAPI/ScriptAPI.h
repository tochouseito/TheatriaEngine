#pragma once
#include "Core/Utility/Components.h"
#include "Core/Utility/InputStruct.h"

#define REGISTER_SCRIPT_FACTORY(SCRIPTNAME) \
    extern "C" __declspec(dllexport) IScript* Create##SCRIPTNAME##Script(GameObject& object) { \
        return new SCRIPTNAME(object); \
    }

struct TransformAPI
{
	// 関数
	Vector3& position() { return data->translation; }
	Vector3& rotation() { return data->degrees; }
	Quaternion& quaternion() { return data->rotation; }
	Scale& scale() { return data->scale; }
	std::function<void(Entity, const Vector3&)> SetPosition;
	std::function<Vector3(Entity)> GetPosition;
	void SetParent(Entity parent) { data->parent = parent; }
private:
	//friend struct ScriptContext;
	friend class GameObject;
	TransformComponent* data = nullptr;
};

struct CameraAPI
{
	// 関数
	float& fovAngleY() { return data->fovAngleY; }
	float& aspectRatio() { return data->aspectRatio; }
	float& nearZ() { return data->nearZ; }
	float& farZ() { return data->farZ; }
private:
	//friend struct ScriptContext;
	friend class GameObject;
	CameraComponent* data = nullptr;
};

struct LineRendererAPI
{
	// 関数
	Vector3& start(uint32_t index) { return (*data)[index].line.start; }
	Vector3& end(uint32_t index) { return (*data)[index].line.end; }
	Color& color(uint32_t index) { return (*data)[index].line.color; }
private:
	//friend struct ScriptContext;
	friend class GameObject;
	std::vector<LineRendererComponent>* data = nullptr;
};

struct Rigidbody2DAPI
{
	// 関数
	class RayCastCallback : public b2RayCastCallback
	{
	public:
		bool hit = false;
		b2Vec2 point;
		b2Vec2 normal;
		float fraction = 1.0f;
		b2Fixture* fixture = nullptr;
		b2Body* body = nullptr;

		float ReportFixture(b2Fixture* a_Fixture, const b2Vec2& a_Point,
			const b2Vec2& a_Normal, float a_Fraction) override
		{
			a_Fixture;
			this->hit = true;
			this->point = a_Point;
			this->normal = a_Normal;
			this->fraction = a_Fraction;
			this->fixture = a_Fixture;
			this->body = a_Fixture->GetBody();
			return a_Fraction; // 最も近いヒットのみ取得
		}
	private:
	};
	Vector2& velocity() { return data->velocity; }

	// 反射方向を計算
	std::function<b2Vec2(const b2Vec2& incident, const b2Vec2& normal)> Reflect;
	// 反射Raycast（指定回数分反射する）
	std::function<b2Vec2(const b2Vec2& start, const b2Vec2& dir,const int ReflectionCount, const float maxLength)> RaycastWithReflectionsOnce;
	// 法線取得（RaycastOnce の直後のみ有効）
	b2Vec2 GetLastHitNormal() const { return m_LastHitNormal; }
	// 瞬間移動
	std::function<void(const Vector2& position)> MovePosition;
	// ライン上の最初にヒットしたオブジェクトを取得
	std::function<GameObject& (const Vector2& start, const Vector2& end)> Linecast;
private:
	//friend struct ScriptContext;
	friend class GameObject;
	Rigidbody2DComponent* data = nullptr;
	// 最後の法線（内部的に保持、ただし状態は保持しないなら静的でもよい）
	b2Vec2 m_LastHitNormal = b2Vec2(0.0f, 1.0f); // 一時的な用途
};

//InputAPI
class InputManager;
struct InputAPI
{
	// 関数
	// キーの押下をチェック
	std::function<bool(const uint8_t& keyNumber)> PushKey;
	// キーのトリガーをチェック
	std::function<bool(const uint8_t& keyNumber)> TriggerKey;
	// 全マウス情報取得
	std::function<const DIMOUSESTATE2& ()> GetAllMouse;
	// マウス移動量を取得
	std::function<MouseMove()> GetMouseMove;
	// マウスの押下をチェック
	std::function<bool(const int32_t& mouseNumber)> IsPressMouse;
	// マウスのトリガーをチェック。押した瞬間だけtrueになる
	std::function<bool(const int32_t& buttonNumber)> IsTriggerMouse;
	// マウスの位置を取得する（ウィンドウ座標系）
	std::function<const Vector2& ()> GetMouseWindowPosition;
	// マウスの位置を取得する（スクリーン座標系）
	std::function<Vector2()> GetMouseScreenPosition;
	// 現在のジョイスティック状態を取得する
	std::function<bool(const int32_t& stickNo, XINPUT_STATE& out)> GetJoystickState;
	// 前回のジョイスティック状態を取得する
	std::function<bool(const int32_t& stickNo, XINPUT_STATE& out)> GetJoystickStatePrevious;
	// デッドゾーンを設定する
	std::function<void(const int32_t& stickNo, const int32_t& deadZoneL, const int32_t& deadZoneR)> SetJoystickDeadZone;
	// 接続されているジョイスティック数を取得する
	std::function<size_t()> GetNumberOfJoysticks;
	// パッドの押されているボタン、スティックの値を取得
	std::function<bool(const PadButton& button, int32_t stickNo)> IsTriggerPadButton;
	std::function<bool(const PadButton& button, int32_t stickNo)> IsPressPadButton;
	std::function<Vector2(const LR& padStick, int32_t stickNo)> GetStickValue;
	std::function<float(const LR& LorR, int32_t stickNo)> GetLRTrigger;
	
private:
	//friend struct ScriptContext;
	friend class GameObject;
	InputManager* data = nullptr;
};
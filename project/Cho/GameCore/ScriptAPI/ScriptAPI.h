#pragma once
#include "Core/Utility/Components.h"
#include "Core/Utility/InputStruct.h"

#define REGISTER_SCRIPT_FACTORY(SCRIPTNAME) \
    extern "C" __declspec(dllexport) IScript* Create##SCRIPTNAME##Script(GameObject& object) { \
        return new SCRIPTNAME(object);}
//// メンバ登録マクロ
//#define REFLECT_SCRIPT_MEMBER(CLASS, MEMBER) \
//    { #MEMBER, \
//      typeid(decltype(CLASS::MEMBER)), \
//      [](void* obj) -> void* { return &static_cast<CLASS*>(obj)->MEMBER; }, \
//      [](void* obj, void* value) { static_cast<CLASS*>(obj)->MEMBER = *static_cast<decltype(CLASS::MEMBER)*>(value); }}
//// 関数登録マクロ
//#define REFLECT_SCRIPT_FUNC(CLASS, FUNC) \
//    { #FUNC, [](void* obj) { static_cast<CLASS*>(obj)->FUNC(); } }
//#define REGISTER_SCRIPT_REFLECTION(SCRIPTNAME) \
//    extern "C" __declspec(dllexport) IScript* Create##SCRIPTNAME##Script(GameObject& object) { \
//        return new SCRIPTNAME(object); \
//    } \
//    extern "C" __declspec(dllexport) const ScriptTypeInfo* GetScriptTypeInfo##SCRIPTNAME() { \
//        static ScriptTypeInfo info = { \
//            #SCRIPTNAME, \
//            SCRIPTNAME::GetReflectedMembers() \
//        }; \
//        return &info; \
//    } \
//    extern "C" __declspec(dllexport) const std::vector<ScriptFunction>* GetScriptFunctions##SCRIPTNAME() { \
//        return &SCRIPTNAME::GetReflectedFunctions(); \
//    }
//#define BEGIN_SCRIPT_REFLECTION(CLASS) \
//    static void RegisterMembers(std::vector<ScriptMember>& out) {
//
//#define ADD_MEMBER(CLASS, MEMBER) \
//    out.emplace_back(REFLECT_SCRIPT_MEMBER(CLASS, MEMBER));
//
//#define END_SCRIPT_REFLECTION_MEMBERS }
//
//#define BEGIN_SCRIPT_FUNCTIONS(CLASS) \
//    static void RegisterFunctions(std::vector<ScriptFunction>& out) {
//
//#define ADD_FUNCTION(FUNC) \
//    out.emplace_back(REFLECT_SCRIPT_FUNC(CLASS, FUNC));
//
//#define END_SCRIPT_REFLECTION_FUNCTIONS }
//
//#define AUTO_REFLECT(CLASS) \
//    static std::vector<ScriptMember> GetReflectedMembers() { return IScript::BuildReflectedMembers<CLASS>(); } \
//    static std::vector<ScriptFunction> GetReflectedFunctions() { return IScript::BuildReflectedFunctions<CLASS>(); }
//
//struct ScriptMember
//{
//	const char* name;
//	std::type_index type;
//	std::function<void* (void*)> getter;
//	std::function<void(void*, void*)> setter;
//};
//
//struct ScriptFunction
//{
//	const char* name;
//	std::function<void(void*)> invoker;
//};
//struct ScriptTypeInfo
//{
//	const char* className;
//	std::vector<ScriptMember> members;
//};
//using GetTypeInfoFn = const ScriptTypeInfo* (*)();
//using GetFuncListFn = const std::vector<ScriptFunction>* (*)();

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
	Vector2& velocity() { return data->velocity; }
	void SetBodyType(b2BodyType type) { data->bodyType = type; }
	void SetFixedRotation(bool fixed) { data->fixedRotation = fixed; }
	void SetActive(bool active) { data->isActive = active; }

	// 反射方向を計算
	std::function<b2Vec2(const b2Vec2& incident, const b2Vec2& normal)> Reflect;
	// 反射Raycast（指定回数分反射する）
	std::function<b2Vec2(const b2Vec2& start, const b2Vec2& dir,const int ReflectionCount, const float maxLength,const std::string hitTag)> RaycastWithReflectionsOnce;
	// 法線取得（RaycastOnce の直後のみ有効）
	b2Vec2 GetLastHitNormal() const { return m_LastHitNormal; }
	// 瞬間移動
	std::function<void(const Vector2& position)> MovePosition;
	// ライン上の最初にヒットしたオブジェクトを取得
	std::function<GameObject& (const Vector2& start, const Vector2& end,const std::string hitTag)> Linecast;
	// 強制的に物理計算
	//std::function<void(bool isAwake)> SetAwake;
private:
	friend class GameObject;
	Rigidbody2DComponent* data = nullptr;
	// 最後の法線（内部的に保持、ただし状態は保持しないなら静的でもよい）
	b2Vec2 m_LastHitNormal = b2Vec2(0.0f, 1.0f); // 一時的な用途
};

// BoxCollider2DAPI
struct BoxCollider2DAPI
{
	// 関数
	float& offsetX() { return data->offsetX; }
	float& offsetY() { return data->offsetY; }
	float& width() { return data->width; }
	float& height() { return data->height; }

	//std::function<bool()> IsSensor;
	//std::function<void(bool isSensor)> SetSensor;
private:
	friend class GameObject;
	BoxCollider2DComponent* data = nullptr;
};

// MaterialAPI
struct MaterialAPI
{
	// 関数
	Color& color() { return data->color; }
	std::wstring& textureName() { return data->textureName; }
private:
	friend class GameObject;
	MaterialComponent* data = nullptr;
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
	friend class GameObject;
	InputManager* data = nullptr;
};

struct EmitterAPI
{
	// 関数
	std::function<void(const Vector3& position)> SetPosition;
	std::function<void(const Vector3& velocity)> SetVelocity;
	std::function<void(const Vector3& acceleration)> SetAcceleration;
	std::function<void(const Vector3& rotation)> SetRotation;
	std::function<void(const Vector3& scale)> SetScale;
	std::function<void(const Vector3& color)> SetColor;
private:
	friend class GameObject;
	EmitterComponent* data = nullptr;
};

struct ParticleAPI
{
	// 関数
	std::function<void(const Vector3& position)> SetPosition;
	std::function<void(const Vector3& velocity)> SetVelocity;
	std::function<void(const Vector3& acceleration)> SetAcceleration;
	std::function<void(const Vector3& rotation)> SetRotation;
	std::function<void(const Vector3& scale)> SetScale;
	std::function<void(const Vector3& color)> SetColor;
private:
	friend class GameObject;
	ParticleComponent* data = nullptr;
};

struct EffectAPI
{
	// 関数
	std::function<void(const Vector3& position)> SetPosition;
	std::function<void(const Vector3& rotation)> SetRotation;
	std::function<void(const Vector3& scale)> SetScale;
	std::function<void(const Vector3& color)> SetColor;
private:
	friend class GameObject;
	EffectComponent* data = nullptr;
};

struct UISpriteAPI
{
	// 関数
	Vector2& position() { return data->position; }
	float& rotation() { return data->rotation; }
	Vector2& scale() { return data->scale; }
	Vector2& anchorPoint() { return data->anchorPoint; }
private:
	friend class GameObject;
	UISpriteComponent* data = nullptr;
};

struct EditorAPI
{
	// 関数
	std::function<void()> Begin;
	std::function<void()> End;
	std::function<void(const char* label, float* v, float v_speed, float v_min, float v_max, const char* format)> DragFloat;
	std::function<void(const char* label, float* v[2], float v_speed, float v_min, float v_max, const char* format)> DragFloat2;
	std::function<void(const char* label, float* v[3], float v_speed, float v_min, float v_max, const char* format)> DragFloat3;
private:
	friend class GameObject;
};

struct AudioAPI
{
	// 関数
	void SetLoop(const bool& loop) { data->isLoop = loop; }
	std::function<void()> Play;
	std::function<void()> Stop;
private:
	friend class GameObject;
	AudioComponent* data = nullptr;
};

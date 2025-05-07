#pragma once
#include <string>
#include "Core/Utility/IDType.h"
#include "GameCore/ScriptAPI/ScriptAPI.h"

using ObjectParameter = std::variant<int, float, bool, Vector3>;
class ECSManager;
class ResourceManager;
class ObjectContainer;

class Prefab 
{
public:
	// Constructor
	Prefab(const Entity& entity,const std::wstring& name,const ObjectType& type) :
		m_Entity(entity),  m_Name(name), m_Type(type)
	{
	}
	// Constructor
	Prefab() {}
	// Destructor
	~Prefab()
	{
	}
	ObjectID GetID() const noexcept { return m_ID; }
	Entity GetEntity() const noexcept { return m_Entity; }
	std::wstring GetName() const noexcept { return m_Name; }
	ObjectType GetType() const noexcept { return m_Type; }
	void SetID(const ObjectID& id) noexcept { m_ID = id; }
private:
	PrefabID m_ID;				// プレハブID
	Entity m_Entity;			// エンティティ
	std::wstring m_Name = L"";	// プレハブ名
	ObjectType m_Type;			// プレハブのタイプ
};
class GameObject
{
	friend class ObjectContainer;
	friend class Add3DObjectCommand;
	friend class AddCameraObjectCommand;
	friend class RenameObjectCommand;
	friend class IScript;
public:
	std::optional<ObjectID> GetID() const noexcept { return m_ID; }
	Entity GetEntity() const noexcept { return m_Entity; }
	std::wstring GetName() const noexcept { return m_Name; }
	ObjectType GetType() const noexcept { return m_Type; }
	std::string GetTag() const noexcept { return m_Tag; }
	void SetTag(std::string_view tag) noexcept { m_Tag = tag.data(); }// この関数はEditorに移す予定
	// オブジェクトが有効かどうか
	bool IsActive() const noexcept
	{
		if (!m_ID) { return false; }
		return m_Active;
	}
	// オブジェクトを有効にする
	void SetActive(bool active) noexcept
	{
		if (!m_ID) { return; }
		m_Active = active;
	}
	// オブジェクトを無効にする
	void SetInactive() noexcept
	{
		if (!m_ID) { return; }
		m_Active = false;
	}

	TransformAPI transform;			// TransformAPI
	CameraAPI camera;				// CameraAPI
	LineRendererAPI lineRenderer;	// LineRendererAPI
	Rigidbody2DAPI rigidbody2D;		// Rigidbody2DAPI
	MaterialAPI material;			// MaterialAPI

	InputAPI input;					// InputAPI

	std::unordered_map<std::string, ObjectParameter> parameters;	// スクリプト用パラメータ
private:
	friend class ScriptGenerateInstanceSystem;
	friend class ScriptInitializeSystem;
	friend class ScriptUpdateSystem;
	friend class ScirptFinalizeSystem;
	friend class CollisionSystem;
	friend class ContactListener2D;

	void SetID(const ObjectID& id) noexcept { m_ID = id; }
	void SetName(const std::wstring& name) noexcept { m_Name = name; }

	std::optional<ObjectID> m_ID = std::nullopt;	// オブジェクトID
	Entity m_Entity;								// エンティティ
	std::wstring m_Name = L"";						// ゲームオブジェクト名
	ObjectType m_Type;								// ゲームオブジェクトのタイプ
	bool m_Active = false;							// アクティブフラグ
	std::string m_Tag = "Default";					// タグ
	ECSManager* m_ECS = nullptr;					// ECSManager
	ResourceManager* m_ResourceManager = nullptr;	// ResourceManager
	InputManager* m_InputManager = nullptr;			// InputManager
	ObjectContainer* m_ObjectContainer = nullptr;	// ObjectContainer

	void Initialize(bool isParentReset = true)
	{
		InitializeTransformAPI(isParentReset);
		InitializeCameraAPI();
		InitializeLineRendererAPI();
		InitializeRigidbody2DAPI();
		InitializeMaterialAPI();
		InitializeInputAPI();
	}

	void InitializeTransformAPI(bool isParentReset = true);
	void InitializeCameraAPI();
	void InitializeLineRendererAPI();
	void InitializeRigidbody2DAPI();
	void InitializeMaterialAPI();
	void InitializeInputAPI();
public:
	// コンストラクタ
	GameObject(ObjectContainer* objectContainer, InputManager* input, ResourceManager* resourceManager, ECSManager* ecs, const Entity& entity, const std::wstring& name, const ObjectType& type) :
		m_ObjectContainer(objectContainer), m_InputManager(input), m_ResourceManager(resourceManager), m_ECS(ecs), m_Entity(entity), m_Name(name), m_Type(type)
	{
		m_Active = true;
	}
	// デフォルトコンストラクタ
	GameObject()
	{
		m_Active = false;
	}
	// デストラクタ
	~GameObject()
	{
	}
	// コピー、代入禁止
	GameObject(const GameObject&) = delete;
	GameObject& operator=(const GameObject&) = delete;
	// ムーブは許可する
	GameObject(GameObject&&) noexcept = default;
	GameObject& operator=(GameObject&&) noexcept = default;
};


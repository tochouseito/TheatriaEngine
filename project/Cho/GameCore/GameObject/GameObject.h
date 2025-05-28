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
// GameObjectクラス
class CHO_API GameObject
{
	friend class GameCore;
	friend class ObjectContainer;
	friend class Add3DObjectCommand;
	friend class AddCameraObjectCommand;
	friend class RenameObjectCommand;
	friend class IScript;
	friend class GameObjectData;
	friend class ScriptGenerateInstanceSystem;
	friend class ScriptInitializeSystem;
	friend class ScriptUpdateSystem;
	friend class ScirptFinalizeSystem;
	friend class CollisionSystem;
	friend class ContactListener2D;
public:
	std::optional<ObjectID> GetID() const noexcept;
	Entity GetEntity() const noexcept { return m_Entity; }
	std::wstring GetName() const noexcept;
	ObjectType GetType() const noexcept { return m_Type; }
	std::string GetTag() const noexcept;
	void SetTag(std::string_view tag) noexcept;// この関数はEditorに移す予定
	// オブジェクトが有効かどうか
	bool IsActive() const noexcept { return m_Active; }
	// オブジェクトを有効にする
	void SetActive(bool active) noexcept { m_Active = active; }
	// オブジェクトを無効にする
	void SetInactive() noexcept { m_Active = false; }
	// スクリプトのインスタンスを取得
	template<typename T>
	T* GetScriptInstance() const noexcept
	{
		// TがIScriptを継承しているか確認
		static_assert(std::is_base_of<IScript, T>::value, "T must be derived from IScript");
		if (m_ECS)
		{
			if (ScriptComponent* script = GetScriptComponent())
			{
				return static_cast<T*>(script->scriptInstance);
			}
		}
		return nullptr;
	}

	TransformAPI transform;			// TransformAPI
	CameraAPI camera;				// CameraAPI
	LineRendererAPI lineRenderer;	// LineRendererAPI
	Rigidbody2DAPI rigidbody2D;		// Rigidbody2DAPI
	BoxCollider2DAPI boxCollider2D;	// BoxCollider2DAPI
	MaterialAPI material;			// MaterialAPI
	UISpriteAPI ui;					// UIAPI
	AudioAPI audio;					// AudioAPI
	InputAPI input;					// InputAPI
	AnimationAPI animation;			// AnimationAPI

	// パラメータを取得
	ObjectParameter GetParameter(const std::string& name) const;
	// パラメータを設定
	void SetParameter(const std::string& name, const ObjectParameter& value);
private:

	void SetID(const ObjectID& id) noexcept;
	void SetName(const std::wstring& name) noexcept;

	
	Entity m_Entity;								// エンティティ
	
	ObjectType m_Type;								// ゲームオブジェクトのタイプ
	bool m_Active = false;							// アクティブフラグ
	
	ECSManager* m_ECS = nullptr;					// ECSManager
	ResourceManager* m_ResourceManager = nullptr;	// ResourceManager
	InputManager* m_InputManager = nullptr;			// InputManager
	ObjectContainer* m_ObjectContainer = nullptr;	// ObjectContainer

	// 実装隠蔽クラス
	class ImplGameObject;
	ImplGameObject* implGameObject = nullptr;

	void Initialize(bool isParentReset = true)
	{
		transform.Initialize(m_Entity, m_ECS, m_ObjectContainer, m_ResourceManager, isParentReset);
		camera.Initialize(m_Entity, m_ECS, m_ObjectContainer, m_ResourceManager);
		lineRenderer.Initialize(m_Entity, m_ECS, m_ObjectContainer, m_ResourceManager);
		rigidbody2D.Initialize(m_Entity, m_ECS, m_ObjectContainer, m_ResourceManager);
		boxCollider2D.Initialize(m_Entity, m_ECS, m_ObjectContainer, m_ResourceManager);
		material.Initialize(m_Entity, m_ECS, m_ObjectContainer, m_ResourceManager);
		ui.Initialize(m_Entity, m_ECS, m_ObjectContainer, m_ResourceManager);
		input.Intialize(m_InputManager);
		audio.Initialize(m_Entity, m_ECS, m_ObjectContainer, m_ResourceManager);
		animation.Initialize(m_Entity, m_ECS, m_ObjectContainer, m_ResourceManager);
	}
	// スクリプトインスタンス取得関数補助
	ScriptComponent* GetScriptComponent() const noexcept;
public:
	// コンストラクタ
	GameObject(ObjectContainer* objectContainer, InputManager* input, ResourceManager* resourceManager, ECSManager* ecs, const Entity& entity, const std::wstring& name, const ObjectType& type);
	// デフォルトコンストラクタ
	GameObject() { m_Active = false; }
	// デストラクタ
	~GameObject();
	// コピー、代入禁止
	GameObject(const GameObject&) = delete;
	GameObject& operator=(const GameObject&) = delete;
	// ムーブは許可する
	GameObject(GameObject&&) noexcept = default;
	GameObject& operator=(GameObject&&) noexcept = default;
};

// GameObjectを生成するためのDataクラス
class GameObjectData
{
	friend class FileSystem;
	friend class ScenePrefab;
	friend class CopyGameObjectCommand;
public:
	GameObjectData(const std::wstring& name, const ObjectType& type) :
		m_Name(name), m_Type(type)
	{
	}
	// GameObject からのコピー用コンストラクタ
	GameObjectData(const GameObject& other);
	~GameObjectData() {}
private:
	std::wstring m_Name = L"";						// ゲームオブジェクト名
	ObjectType m_Type;								// ゲームオブジェクトのタイプ
	std::string m_Tag = "Default";					// タグ
	// コンポーネント
	std::optional<TransformComponent> m_Transform = std::nullopt;
	std::optional<CameraComponent> m_Camera = std::nullopt;
	std::optional<MeshFilterComponent> m_MeshFilter = std::nullopt;
	std::optional<MeshRendererComponent> m_MeshRenderer = std::nullopt;
	std::optional<ScriptComponent> m_Script = std::nullopt;
	std::vector<LineRendererComponent> m_LineRenderer;
	std::optional<MaterialComponent> m_Material = std::nullopt;
	std::optional<Rigidbody2DComponent> m_Rigidbody2D = std::nullopt;
	std::optional<BoxCollider2DComponent> m_BoxCollider2D = std::nullopt;
	std::optional<EmitterComponent> m_Emitter = std::nullopt;
	std::optional<ParticleComponent> m_Particle = std::nullopt;
	std::optional<UISpriteComponent> m_UISprite = std::nullopt;
	std::optional<LightComponent> m_Light = std::nullopt;
	std::optional<AudioComponent> m_Audio = std::nullopt;
	std::optional<AnimationComponent> m_Animation = std::nullopt;
};


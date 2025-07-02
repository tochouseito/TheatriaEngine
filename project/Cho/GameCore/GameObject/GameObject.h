#pragma once
#include <string>
#include "Core/Utility/IDType.h"
#include "GameCore/ScriptAPI/ScriptAPI.h"


using ObjectParameter = std::variant<int, float, bool, Vector3>;
class ECSManager;
class ResourceManager;
class ObjectContainer;

// GameObjectクラス
class CHO_API GameObject
{
public:
	std::optional<Entity> GetSrcEntity() const noexcept;
	ObjectHandle GetHandle() const noexcept { return m_Handle; }
	std::wstring GetName() const noexcept;
	ObjectType GetType() const noexcept { return m_Type; }
	std::string GetTag() const noexcept;
	std::wstring GetCurrentSceneName() const noexcept;
	void SetCurrentSceneName(const std::wstring& name) noexcept;
	void SetTag(std::string_view tag) noexcept;// この関数はEditorに移す予定
	void SetType(const ObjectType& type) noexcept { m_Type = type; }
	void SetName(const std::wstring& name) noexcept;
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
		// TがMarionnetteを継承しているか確認
		static_assert(std::is_base_of<Marionnette, T>::value, "T must be derived from Marionnette");
		if (ScriptComponent* script = GetScriptComponent())
		{
			return static_cast<T*>(script->instance);
		}
		return nullptr;
	}

	//TransformAPI transform;			// TransformAPI
	//CameraAPI camera;				// CameraAPI
	//LineRendererAPI lineRenderer;	// LineRendererAPI
	//Rigidbody2DAPI rigidbody2D;		// Rigidbody2DAPI
	//BoxCollider2DAPI boxCollider2D;	// BoxCollider2DAPI
	//MaterialAPI material;			// MaterialAPI
	//UISpriteAPI ui;					// UIAPI
	//AudioAPI audio;					// AudioAPI
	//InputAPI input;					// InputAPI
	//AnimationAPI animation;			// AnimationAPI
	//ParticleAPI particle;		// ParticleAPI

	// パラメータを取得
	ObjectParameter GetParameter(const std::string& name) const;
	// パラメータを設定
	void SetParameter(const std::string& name, const ObjectParameter& value);
private:
	ObjectHandle m_Handle;			// オブジェクトハンドル
	ObjectType m_Type;								// ゲームオブジェクトのタイプ
	bool m_Active = false;							// アクティブフラグ

	// 実装隠蔽クラス
	class ImplGameObject;
	ImplGameObject* implGameObject = nullptr;

	/*void Initialize(bool isParentReset = true)
	{
		transform.Initialize(m_Entity, m_ECS, m_ObjectContainer, m_ResourceManager, isParentReset);
		camera.Initialize(m_Entity, m_ECS, m_ObjectContainer, m_ResourceManager);
		lineRenderer.Initialize(m_Entity, m_ECS, m_ObjectContainer, m_ResourceManager);
		rigidbody2D.Initialize(m_Entity, m_ECS, m_ObjectContainer, m_ResourceManager);
		boxCollider2D.Initialize(m_Entity, m_ECS, m_ObjectContainer, m_ResourceManager);
		material.Initialize(m_Entity, m_ECS, m_ObjectContainer, m_ResourceManager);
		ui.Initialize(m_Entity, m_ECS, m_ObjectContainer, m_ResourceManager);
		input.Intialize(m_InputManager);
		particle.Initialize(m_Entity, m_ECS, m_ObjectContainer, m_ResourceManager);
		audio.Initialize(m_Entity, m_ECS, m_ObjectContainer, m_ResourceManager);
		animation.Initialize(m_Entity, m_ECS, m_ObjectContainer, m_ResourceManager);
	}*/
	// スクリプトインスタンス取得関数補助
	ScriptComponent* GetScriptComponent() const noexcept;
public:
	// コンストラクタ
	GameObject(
		const ObjectHandle& handle,
		const std::wstring& name,
		const ObjectType& type);
	// デストラクタ
	~GameObject();
	// コピー、代入禁止
	GameObject(const GameObject&) = delete;
	GameObject& operator=(const GameObject&) = delete;
	// ムーブは許可する
	GameObject(GameObject&&) noexcept = default;
	GameObject& operator=(GameObject&&) noexcept = default;
};

//// IDと生成したSceneと紐づけるハンドル
//struct GameObjectHandle
//{
//	uint32_t id{};
//	std::optional<uint32_t> generation{};
//};

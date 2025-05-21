#include "pch.h"
#include "GameObject.h"
#include "GameCore/ECS/ECSManager.h"
#include "Platform/InputManager/InputManager.h"
#include "GameCore/ObjectContainer/ObjectContainer.h"
#include "GameCore/PhysicsEngine/PhysicsEngine.h"
#include "Resources/ResourceManager/ResourceManager.h"

class GameObject::ImplGameObject
{
public:
	ImplGameObject() = default;
	~ImplGameObject() = default;
	std::function<std::optional<ObjectID>()> GetIDFunc;	// スクリプト用ゲームオブジェクトID取得関数
	std::function<void(const std::optional<ObjectID>& id)> SetIDFunc;	// スクリプト用ゲームオブジェクトID設定関数
	std::function<std::wstring()> GetNameFunc;	// スクリプト用ゲームオブジェクト名取得関数
	std::function<void(const std::wstring& name)> SetNameFunc;	// スクリプト用ゲームオブジェクト名設定関数
	std::function<std::string()> GetTagFunc;	// スクリプト用タグ取得関数
	std::function<void(const std::string& tag)> SetTagFunc;	// スクリプト用タグ設定関数
	std::function<ObjectParameter(const std::string& name)> GetParameterFunc;	// スクリプト用パラメータ取得関数
	std::function<void(const std::string& name, const ObjectParameter& value)> SetParameterFunc;	// スクリプト用パラメータ設定関数
	// オブジェクトID
	std::optional<ObjectID> m_ID = std::nullopt;
	// ゲームオブジェクト名
	std::wstring m_Name = L"";
	// タグ
	std::string m_Tag = "Default";
	// スクリプト用パラメータ
	std::unordered_map<std::string, ObjectParameter> parameters;
};

std::optional<ObjectID> GameObject::GetID() const noexcept
{
	if (implGameObject->GetIDFunc)
	{
		return implGameObject->GetIDFunc();
	}
	return std::nullopt;
}

std::wstring GameObject::GetName() const noexcept
{
	if (implGameObject->GetNameFunc)
	{
		return implGameObject->GetNameFunc();
	}
	return L"";
}

std::string GameObject::GetTag() const noexcept
{
	if (implGameObject->GetTagFunc)
	{
		return implGameObject->GetTagFunc();
	}
	return "Default";
}

void GameObject::SetTag(std::string_view tag) noexcept
{
	if (implGameObject->SetTagFunc)
	{
		implGameObject->SetTagFunc(tag.data());
	}
	return;
}

ObjectParameter GameObject::GetParameter(const std::string& name) const
{
	if (implGameObject->GetParameterFunc)
	{
		return implGameObject->GetParameterFunc(name);
	}
	return ObjectParameter();
}

void GameObject::SetParameter(const std::string& name, const ObjectParameter& value)
{
	if (implGameObject->SetParameterFunc)
	{
		implGameObject->SetParameterFunc(name, value);
	}
}

void GameObject::SetID(const ObjectID& id) noexcept
{
	implGameObject->SetIDFunc(id);
}

void GameObject::SetName(const std::wstring& name) noexcept
{
	implGameObject->SetNameFunc(name);
}

GameObject::GameObject(ObjectContainer* objectContainer, InputManager* input, ResourceManager* resourceManager, ECSManager* ecs, const Entity& entity, const std::wstring& name, const ObjectType& type) :
	m_ObjectContainer(objectContainer), m_InputManager(input), m_ResourceManager(resourceManager), m_ECS(ecs), m_Entity(entity), m_Type(type)
{
	m_Active = true;
	implGameObject = new ImplGameObject();
	implGameObject->GetIDFunc = [this]() {
		return implGameObject->m_ID;
		};
	implGameObject->SetIDFunc = [this](const std::optional<ObjectID>& id) {
		implGameObject->m_ID = id;
		};
	implGameObject->GetNameFunc = [this]() {
		return implGameObject->m_Name;
		};
	implGameObject->SetNameFunc = [this](const std::wstring& name) {
		implGameObject->m_Name = name;
		};
	implGameObject->GetTagFunc = [this]() {
		return implGameObject->m_Tag;
		};
	implGameObject->SetTagFunc = [this](const std::string& tag) {
		implGameObject->m_Tag = tag;
		};
	implGameObject->GetParameterFunc = [this](const std::string& name) {
		ObjectParameter value;
		value = implGameObject->parameters[name];
		return value;
		};
	implGameObject->SetParameterFunc = [this](const std::string& name, const ObjectParameter& value) {
		implGameObject->parameters[name] = value;
		};
	// ゲームオブジェクトの名前を設定
	implGameObject->SetNameFunc(name);
}

GameObject::~GameObject()
{
	delete implGameObject;
}

GameObjectData::GameObjectData(const GameObject& other)
{
	this->m_Name = other.implGameObject->GetNameFunc();
	this->m_Type = other.m_Type;
	this->m_Tag = other.implGameObject->GetTagFunc();

	Entity e = other.m_Entity;

	if (auto* transform = other.m_ECS->GetComponent<TransformComponent>(e))
	{
		m_Transform = *transform;
	}
	if (auto* camera = other.m_ECS->GetComponent<CameraComponent>(e))
	{
		m_Camera = *camera;
	}
	if (auto* mesh = other.m_ECS->GetComponent<MeshFilterComponent>(e))
	{
		m_MeshFilter = *mesh;
	}
	if (auto* renderer = other.m_ECS->GetComponent<MeshRendererComponent>(e))
	{
		m_MeshRenderer = *renderer;
	}
	if (auto* script = other.m_ECS->GetComponent<ScriptComponent>(e))
	{
		m_Script = *script;
	}
	if (auto* material = other.m_ECS->GetComponent<MaterialComponent>(e))
	{
		m_Material = *material;
	}
	if (auto* rigidbody2D = other.m_ECS->GetComponent<Rigidbody2DComponent>(e))
	{
		m_Rigidbody2D = *rigidbody2D;
	}
	if (auto* boxCollider2D = other.m_ECS->GetComponent<BoxCollider2DComponent>(e))
	{
		m_BoxCollider2D = *boxCollider2D;
	}
	if (auto* emitter = other.m_ECS->GetComponent<EmitterComponent>(e))
	{
		m_Emitter = *emitter;
	}
	if (auto* particle = other.m_ECS->GetComponent<ParticleComponent>(e))
	{
		m_Particle = *particle;
	}
	if (auto* uiSprite = other.m_ECS->GetComponent<UISpriteComponent>(e))
	{
		m_UISprite = *uiSprite;
	}
	if (auto* light = other.m_ECS->GetComponent<LightComponent>(e))
	{
		m_Light = *light;
	}
	if (auto* audio = other.m_ECS->GetComponent<AudioComponent>(e))
	{
		m_Audio = *audio;
	}

	if (auto* lineRenderer = other.m_ECS->GetAllComponents<LineRendererComponent>(e))
	{
		m_LineRenderer.clear();
		for (const auto& line : *lineRenderer)
		{
			m_LineRenderer.push_back(line);
		}
		std::reverse(m_LineRenderer.begin(), m_LineRenderer.end());
	}
}



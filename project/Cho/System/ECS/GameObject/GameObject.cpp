#include "PrecompiledHeader.h"
#include "GameObject.h"

GameObject::~GameObject()
{
    // すべてのコンポーネントを削除
    componentManager->RemoveComponent(entity);
    // エンティティを返還
	entityManager->RemoveEntity(entity);
}

void GameObject::CreateEntity()
{
    entity = entityManager->CreateEntity();
}

void GameObject::SetManager(EntityManager* em, ComponentManager* cm)
{
    entityManager = em;
    componentManager = cm;
}

void GameObject::SetParent(GameObject* newParent) {
    parent = newParent;
    newParent->AddChild(this);
}

void GameObject::AddChild(GameObject* child) {
    children.push_back(child);
}

Entity GameObject::GetEntityID() const {
    return entity;
}

ObjectType GameObject::GetObjectType() const
{
    return type;
}

std::string GameObject::GetName() const
{
    return name;
}

void GameObject::UpdateChildren(float deltaTime) {
    // 子オブジェクトの更新
    for (auto child : children) {
        child->UpdateChildren(deltaTime);
    }
}

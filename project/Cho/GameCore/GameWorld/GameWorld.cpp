#include "pch.h"
#include "GameWorld.h"

ObjectID GameWorld::AddGameObject(GameObject obj)
{
    ObjectID id = static_cast<ObjectID>(m_pGameObjects.push_back(FVector<std::unique_ptr<GameObject>>()));
	obj.SetID(id);
	m_pGameObjects[id].push_back(std::make_unique<GameObject>(std::move(obj)));
	return id;
}

void GameWorld::AddGameObjectClone(const ObjectID& srcID, GameObject clone)
{
	if (m_pGameObjects.isValid(srcID) &&
		clone.GetSrcID().has_value() &&
		clone.GetSrcID().value() == srcID)
	{
		m_pGameObjects[srcID].push_back(std::make_unique<GameObject>(std::move(clone)));
	}
}

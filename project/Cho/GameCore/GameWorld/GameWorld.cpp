#include "pch.h"
#include "GameWorld.h"

void GameWorld::AddGameObject(GameObject obj)
{
    uint32_t id = static_cast<uint32_t>(m_pGameObjects.push_back(FVector<FVector<std::unique_ptr<GameObject>>>()));
	if (id != obj.GetEntity())
	{
		Log::Write(LogLevel::Assert, "GameWorldID mismatch Entity");
	}
	m_pGameObjects[id][0].push_back(std::make_unique<GameObject>(std::move(obj)));
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

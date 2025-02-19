#include "PrecompiledHeader.h"
#include "EntityManager.h"

// 新しいエンティティを作成し、そのエンティティIDを返します。
Entity EntityManager::CreateEntity() {
    Entity entity;

	// 空いているエンティティがある場合はそれを使います。
	if (!freeEntities.empty()) {
		entity = freeEntities.back();
		freeEntities.pop_back();
		activeEntities.push_back(entity);  // アクティブなエンティティリストに追加します。
	}
	else
	{
		entity = nextEntityID;  // 次のエンティティIDを生成します。
		nextEntityID++;
		activeEntities.push_back(entity);  // アクティブなエンティティリストに追加します。
	}
    return entity;
}

// 指定されたエンティティを破棄します。
void EntityManager::RemoveEntity(Entity entity) {
    // 指定されたエンティティをアクティブリストから削除します。
    activeEntities.erase(std::remove(activeEntities.begin(), activeEntities.end(), entity), activeEntities.end());
	// 空いているエンティティリストに追加します。
	freeEntities.push_back(entity);
}

void EntityManager::SetCameraID(Entity entity)
{
    nowCameraID = entity;
}

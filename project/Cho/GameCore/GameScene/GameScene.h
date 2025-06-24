#pragma once
#include "GameCore/GameObject/GameObject.h"
#include "GameCore/Prefab/Prefab.h"
#include "Core/Utility/FVector.h"

class GameScene
{
public:
	// Constructor
	GameScene(const std::wstring& sceneName) : m_SceneName(sceneName) {}
	// Destructor
	~GameScene() = default;

	void AddPrefab(const Prefab& prefab);
	void RemovePrefab(const std::wstring& prefabName);
	void RemovePrefab(const size_t idx);
	void ClearPrefabs() { m_Prefabs.clear(); m_PrefabNameToIdxMap.clear(); }
	void SetSceneName(const std::wstring& sceneName) { m_SceneName = sceneName; }
	void SetStartCameraName(const std::wstring& cameraName) { m_StartCameraName = cameraName; }
	std::vector<Prefab> GetPrefabs() { return m_Prefabs.GetVector(); }
	std::wstring GetSceneName() const { return m_SceneName; }
	std::wstring GetStartCameraName() const { return m_StartCameraName; }
	std::unordered_map<std::wstring, size_t> GetPrefabNameToIdxMap() const { return m_PrefabNameToIdxMap; }
private:
	std::wstring m_SceneName = L"";
	FVector<Prefab> m_Prefabs;
	std::unordered_map<std::wstring, size_t> m_PrefabNameToIdxMap;
	std::wstring m_StartCameraName = L"";
};


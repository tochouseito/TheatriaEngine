#pragma once
class ECSManager;
class SystemManager;
class ObjectContainer;
class SceneManager;
class GameCoreCommand
{
public:
	// Constructor
	GameCoreCommand(SceneManager* sceneManager, ECSManager* ecsManager, SystemManager* systemManager, ObjectContainer* objectContainer) :
		m_SceneManager(sceneManager),
		m_ECSManager(ecsManager),
		m_SystemManager(systemManager),
		m_ObjectContainer(objectContainer)
	{
	}
	// Destructor
	~GameCoreCommand()
	{
	}
private:
	SceneManager* m_SceneManager = nullptr;
	ECSManager* m_ECSManager = nullptr;
	SystemManager* m_SystemManager = nullptr;
	ObjectContainer* m_ObjectContainer = nullptr;
};


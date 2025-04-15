#pragma once
class GameCore;
class ResourceManager;
class GraphicsEngine;
// インタフェース
//class ICommand
//{
//public:
//	// Constructor
//	ICommand() = default;
//	// Destructor
//	virtual ~ICommand() = default;
//	virtual void Execute(EngineCommand* edit) = 0;
//	virtual void Undo(EngineCommand* edit) = 0;
//};
class EngineCommand
{
public:
	// Constructor
	EngineCommand(GameCore* gameCore, ResourceManager* resourceManager, GraphicsEngine* graphicsEngine) :
		m_GameCore(gameCore),
		m_ResourceManager(resourceManager),
		m_GraphicsEngine(graphicsEngine)
	{
	}
	// Destructor
	~EngineCommand()
	{
	}
private:
	GameCore* m_GameCore = nullptr;
	ResourceManager* m_ResourceManager = nullptr;
	GraphicsEngine* m_GraphicsEngine = nullptr;
};


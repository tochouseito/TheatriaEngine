#pragma once
#include <memory>
#include <vector>
class EditorCommand;
class ICommand
{
public:
	// Constructor
	ICommand() = default;
	// Destructor
	virtual ~ICommand() = default;
	virtual void Execute(EditorCommand* edit) = 0;
	virtual void Undo(EditorCommand* edit) = 0;
};

class AddGameObjectCommand :public ICommand
{
public:
	void Execute(EditorCommand* edit)override;
	void Undo(EditorCommand* edit)override;
};

class ResourceManager;
class GraphicsEngine;
class GameCore;
class EditorCommand
{
public:
	// Constructor
	EditorCommand(ResourceManager* resourceManager,GraphicsEngine* graphicsEngine,GameCore* gameCore):
		m_ResourceManager(resourceManager), m_GraphicsEngine(graphicsEngine), m_GameCore(gameCore)
	{
	}
	// Destructor
	~EditorCommand()
	{
	}
	void ExecuteCommand(std::unique_ptr<ICommand> command)
	{
		command->Execute(this);
		m_Commands.push_back(std::move(command));
	}
	void Undo()
	{
		if (!m_Commands.empty())
		{
			m_Commands.back()->Undo(this);
			m_Commands.pop_back();
		}
	};
	ResourceManager* GetResourceManagerPtr() { return m_ResourceManager; }
	GraphicsEngine* GetGraphicsEnginePtr() { return m_GraphicsEngine; }
	GameCore* GetGameCorePtr() { return m_GameCore; }
private:
	std::vector<std::unique_ptr<ICommand>> m_Commands;
	ResourceManager* m_ResourceManager = nullptr;
	GraphicsEngine* m_GraphicsEngine = nullptr;
	GameCore* m_GameCore = nullptr;
};


#pragma once
#include <memory>
#include <vector>
class ICommand
{
public:
	// Constructor
	ICommand() = default;
	// Destructor
	virtual ~ICommand() = default;
	virtual void Execute() = 0;
	virtual void Undo() = 0;
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
		command->Execute();
		m_Commands.push_back(std::move(command));
	}
	void Undo()
	{
		if (!m_Commands.empty())
		{
			m_Commands.back()->Undo();
			m_Commands.pop_back();
		}
	};
private:
	std::vector<std::unique_ptr<ICommand>> m_Commands;
	ResourceManager* m_ResourceManager = nullptr;
	GraphicsEngine* m_GraphicsEngine = nullptr;
	GameCore* m_GameCore = nullptr;
};


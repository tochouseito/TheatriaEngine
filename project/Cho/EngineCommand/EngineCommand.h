#pragma once
#include <vector>
class GameCore;
class ResourceManager;
class GraphicsEngine;
// インタフェース
class IEngineCommand
{
public:
	// Constructor
	IEngineCommand() = default;
	// Destructor
	virtual ~IEngineCommand() = default;
	virtual void Execute(EngineCommand* edit) = 0;
	virtual void Undo(EngineCommand* edit) = 0;
};
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
	// コマンド実行
	void ExecuteCommand(std::unique_ptr<IEngineCommand> command)
	{
		command->Execute(this);
		m_Commands.push_back(std::move(command));
	}
	// コマンド取り消し
	void UndoCommand()
	{
		if (!m_Commands.empty())
		{
			m_Commands.back()->Undo(this);
			m_Commands.pop_back();
		}
	}
private:
	GameCore* m_GameCore = nullptr;
	ResourceManager* m_ResourceManager = nullptr;
	GraphicsEngine* m_GraphicsEngine = nullptr;

	std::vector<std::unique_ptr<IEngineCommand>> m_Commands;
};


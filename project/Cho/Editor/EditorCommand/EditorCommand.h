#pragma once
#include "SDK/DirectX/DirectX12/stdafx/stdafx.h"
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
// Test:GameObjectを追加する
class AddGameObjectCommand :public ICommand
{
public:
	AddGameObjectCommand(const std::wstring& name,const uint32_t& type)
		:m_Name(name), m_Type(type)
	{
	}
	void Execute(EditorCommand* edit)override;
	void Undo(EditorCommand* edit)override;
private:
	std::wstring m_Name;
	uint32_t m_Type;
};
class AddTransformComponent :public ICommand
{
public:
	AddTransformComponent(const uint32_t& entity):
		m_Entity(entity)
	{

	}
	void Execute(EditorCommand* edit)override;
	void Undo(EditorCommand* edit)override;
private:
	uint32_t m_Entity;
	uint32_t m_MapID;
};
class AddMeshComponent :public ICommand
{
public:
	AddMeshComponent(const uint32_t& entity) :
		m_Entity(entity)
	{
	}
	void Execute(EditorCommand* edit)override;
	void Undo(EditorCommand* edit)override;
private:
	uint32_t m_Entity;
};
class AddRenderComponent :public ICommand
{
public:
	AddRenderComponent(const uint32_t& entity) :
		m_Entity(entity)
	{
	}
	void Execute(EditorCommand* edit)override;
	void Undo(EditorCommand* edit)override;
private:
	uint32_t m_Entity;
};
class AddCameraComponent :public ICommand
{
public:
	AddCameraComponent(const uint32_t& entity) :
		m_Entity(entity)
	{
	}
	void Execute(EditorCommand* edit)override;
	void Undo(EditorCommand* edit)override;
private:
	uint32_t m_Entity;
};
class SetMainCamera :public ICommand
{
public:
	SetMainCamera(const uint32_t& setCameraID) :
		m_SetCameraID(setCameraID)
	{
	}
	void Execute(EditorCommand* edit)override;
	void Undo(EditorCommand* edit)override;
private:
	std::optional<uint32_t> m_PreCameraID = std::nullopt;
	std::optional<uint32_t> m_SetCameraID = std::nullopt;
};

class ResourceManager;
class GraphicsEngine;
class GameCoreCommand;
class EditorCommand
{
public:
	// Constructor
	EditorCommand(ResourceManager* resourceManager,GraphicsEngine* graphicsEngine,GameCoreCommand* gameCoreCommand):
		m_ResourceManager(resourceManager), m_GraphicsEngine(graphicsEngine), m_GameCoreCommand(gameCoreCommand)
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
	GameCoreCommand* GetGameCoreCommandPtr() { return m_GameCoreCommand; }

	// レンダリングテクスチャのハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetGameTextureHandle();
	D3D12_GPU_DESCRIPTOR_HANDLE GetSceneTextureHandle();
private:
	std::vector<std::unique_ptr<ICommand>> m_Commands;
	ResourceManager* m_ResourceManager = nullptr;
	GraphicsEngine* m_GraphicsEngine = nullptr;
	GameCoreCommand* m_GameCoreCommand = nullptr;
};


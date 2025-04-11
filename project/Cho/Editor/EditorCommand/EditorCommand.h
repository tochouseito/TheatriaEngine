#pragma once
#include "SDK/DirectX/DirectX12/stdafx/stdafx.h"
#include "GameCore/SingleSystemManager/SingleSystemManager.h"
#include "GameCore/MultiSystemManager/MultiSystemManager.h"
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
// 3Dオブジェクトを追加するコマンド
class Add3DObjectCommand :public ICommand
{
public:
	Add3DObjectCommand()
	{
	}
	void Execute(EditorCommand* edit)override;
	void Undo(EditorCommand* edit)override;
private:
	uint32_t m_MapID;
};
// カメラオブジェクトを追加するコマンド
class AddCameraObjectCommand :public ICommand
{
public:
	AddCameraObjectCommand()
	{
	}
	void Execute(EditorCommand* edit)override;
	void Undo(EditorCommand* edit)override;
private:
	uint32_t m_MapID;
};
// MeshFilterComponentを追加するコマンド
class AddMeshFilterComponent :public ICommand
{
public:
	AddMeshFilterComponent(const uint32_t& entity) :
		m_Entity(entity)
	{
	}
	void Execute(EditorCommand* edit)override;
	void Undo(EditorCommand* edit)override;
private:
	uint32_t m_Entity;
};
class AddMeshRendererComponent :public ICommand
{
public:
	AddMeshRendererComponent(const uint32_t& entity) :
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
// スクリプトコンポーネントを追加するコマンド
class AddScriptComponent :public ICommand
{
public:
	AddScriptComponent(const uint32_t& entity) :
		m_Entity(entity)
	{
	}
	void Execute(EditorCommand* edit)override;
	void Undo(EditorCommand* edit)override;
private:
	uint32_t m_Entity;
};
// ラインレンダラーコンポーネントを追加するコマンド
class AddLineRendererComponent :public ICommand
{
public:
	AddLineRendererComponent(const uint32_t& entity) :
		m_Entity(entity)
	{
	}
	void Execute(EditorCommand* edit)override;
	void Undo(EditorCommand* edit)override;
private:
	uint32_t m_Entity;
	uint32_t m_MapID;
};
// 2DRigidbodyコンポーネントを追加するコマンド
class AddRigidbody2DComponent :public ICommand
{
public:
	AddRigidbody2DComponent(const uint32_t& entity) :
		m_Entity(entity)
	{
	}
	void Execute(EditorCommand* edit)override;
	void Undo(EditorCommand* edit)override;
private:
	uint32_t m_Entity;
};
// BoxCollider2Dコンポーネントを追加するコマンド
class AddBoxCollider2DComponent :public ICommand
{
public:
	AddBoxCollider2DComponent(const uint32_t& entity) :
		m_Entity(entity)
	{
	}
	void Execute(EditorCommand* edit)override;
	void Undo(EditorCommand* edit)override;
private:
	uint32_t m_Entity;
};

class ResourceManager;
class GraphicsEngine;
class GameCoreCommand;
class GameObject;
class EditorCommand
{
public:
	// Constructor
	EditorCommand(ResourceManager* resourceManager,GraphicsEngine* graphicsEngine,GameCoreCommand* gameCoreCommand):
		m_ResourceManager(resourceManager), m_GraphicsEngine(graphicsEngine), m_GameCoreCommand(gameCoreCommand)
	{
		m_pSingleSystem = std::make_unique<SingleSystemManager>();
		m_pMultiSystem = std::make_unique<MultiSystemManager>();
		CreateSystem();
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
	// エディタの選択中オブジェクトを取得
	GameObject* GetSelectedObject() { return m_SelectedObject; }
	// エディタの選択中オブジェクトをセット
	void SetSelectedObject(GameObject* object) { m_SelectedObject = object; }
	// プロジェクトの保存
	void SaveProjectFile(const std::wstring& projectName);
	// Test:スクリプト作成	
	void GenerateScript(const std::string& scriptName);
	// ゲーム再生
	void GameRun();
	// ゲーム停止
	void GameStop();
	// シーンのエディタ更新
	void UpdateEditorScene();
private:
	void CreateSystem();

	std::vector<std::unique_ptr<ICommand>> m_Commands;
	ResourceManager* m_ResourceManager = nullptr;
	GraphicsEngine* m_GraphicsEngine = nullptr;
	GameCoreCommand* m_GameCoreCommand = nullptr;
	GameObject* m_SelectedObject = nullptr;
	// エディタの更新システム
	std::unique_ptr<SingleSystemManager> m_pSingleSystem = nullptr;
	std::unique_ptr<MultiSystemManager> m_pMultiSystem = nullptr;
};


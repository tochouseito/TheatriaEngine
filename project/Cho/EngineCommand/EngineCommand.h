#pragma once
#include "SDK/DirectX/DirectX12/stdafx/stdafx.h"
class GameCore;
class ResourceManager;
class GraphicsEngine;
class GameObject;
class EngineCommand;
// インタフェース
class IEngineCommand
{
public:
	// Constructor
	IEngineCommand() = default;
	// Destructor
	virtual ~IEngineCommand() = default;
	virtual bool Execute(EngineCommand* edit) = 0;
	virtual bool Undo(EngineCommand* edit) = 0;
};
class EngineCommand
{
	// Command クラス
	friend class Add3DObjectCommand;
	friend class AddCameraObjectCommand;
	friend class AddMeshFilterComponent;
	friend class AddMeshRendererComponent;
	friend class SetMainCamera;
	friend class AddScriptComponent;
	friend class AddLineRendererComponent;
	friend class AddRigidbody2DComponent;
	friend class AddBoxCollider2DComponent;
	friend class DeleteObjectCommand;
	friend class RenameObjectCommand;
	friend class AddMaterialComponent;
	// Editor

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
	bool ExecuteCommand(std::unique_ptr<IEngineCommand> command)
	{
		if (command->Execute(this))
		{
			m_Commands.push_back(std::move(command));
			return true;
		}
		return false;
	}
	// コマンド取り消し
	bool UndoCommand()
	{
		if (!m_Commands.empty())
		{
			if (m_Commands.back()->Undo(this))
			{
				m_Commands.pop_back();
				return true;
			}
			// 取り消しに失敗した場合は、コマンドを削除しない
			return false;
		}
		return false;
	}
	// コマンドクリア
	void ClearCommands()
	{
		m_Commands.clear();
	}
	// ゲームコアを取得
	GameCore* GetGameCore() { return m_GameCore; }
	// リソースマネージャを取得
	ResourceManager* GetResourceManager() { return m_ResourceManager; }
	// グラフィックスエンジンを取得
	GraphicsEngine* GetGraphicsEngine() { return m_GraphicsEngine; }
	// エディタの選択中オブジェクトを取得
	GameObject* GetSelectedObject() { return m_SelectedObject; }
	// エディタの選択中オブジェクトをセット
	void SetSelectedObject(GameObject* object) { m_SelectedObject = object; }
	// レンダリングテクスチャのハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetGameTextureHandle();
	D3D12_GPU_DESCRIPTOR_HANDLE GetSceneTextureHandle();
	D3D12_GPU_DESCRIPTOR_HANDLE GetEffectEditTextureHandle();
	// テクスチャを取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandle(const std::wstring& name);
	// プロジェクトの保存
	void SaveProjectFile(const std::wstring& projectName);
	// Test:スクリプト作成	
	void GenerateScript(const std::string& scriptName);
	// ゲーム再生
	void GameRun();
	// ゲーム停止
	void GameStop();
	// ゲーム実行中か
	bool IsGameRunning();
	// ゲームの解像度を取得
	UINT64 GetGameResolutionX() const;
	UINT GetGameResolutionY() const;
private:
	// 選択中のオブジェクト
	GameObject* m_SelectedObject = nullptr;

	GameCore* m_GameCore = nullptr;
	ResourceManager* m_ResourceManager = nullptr;
	GraphicsEngine* m_GraphicsEngine = nullptr;
	std::vector<std::unique_ptr<IEngineCommand>> m_Commands;
};


#pragma once
#include <optional>
#include "EngineCommand/EngineCommand.h"

// 3Dオブジェクトを追加するコマンド
class Add3DObjectCommand :public IEngineCommand
{
public:
	Add3DObjectCommand()
	{
	}
	void Execute(EngineCommand* edit)override;
	void Undo(EngineCommand* edit)override;
private:
	uint32_t objectID;
};
// カメラオブジェクトを追加するコマンド
class AddCameraObjectCommand :public IEngineCommand
{
public:
	AddCameraObjectCommand()
	{
	}
	void Execute(EngineCommand* edit)override;
	void Undo(EngineCommand* edit)override;
private:
	uint32_t objectID;
};
// MeshFilterComponentを追加するコマンド
class AddMeshFilterComponent :public IEngineCommand
{
public:
	AddMeshFilterComponent(const uint32_t& entity) :
		m_Entity(entity)
	{
	}
	void Execute(EngineCommand* edit)override;
	void Undo(EngineCommand* edit)override;
private:
	uint32_t m_Entity;
};
// MeshRendererComponentを追加するコマンド
class AddMeshRendererComponent :public IEngineCommand
{
public:
	AddMeshRendererComponent(const uint32_t& entity) :
		m_Entity(entity)
	{
	}
	void Execute(EngineCommand* edit)override;
	void Undo(EngineCommand* edit)override;
private:
	uint32_t m_Entity;
};
// MainCameraを設定するコマンド
class SetMainCamera :public IEngineCommand
{
public:
	SetMainCamera()
	{
	}
	void Execute(EngineCommand* edit)override;
	void Undo(EngineCommand* edit)override;
private:
	std::optional<uint32_t> m_PreCameraID = std::nullopt;
};
// スクリプトコンポーネントを追加するコマンド
class AddScriptComponent :public IEngineCommand
{
public:
	AddScriptComponent(const uint32_t& entity) :
		m_Entity(entity)
	{
	}
	void Execute(EngineCommand* edit)override;
	void Undo(EngineCommand* edit)override;
private:
	uint32_t m_Entity;
};
// ラインレンダラーコンポーネントを追加するコマンド
class AddLineRendererComponent :public IEngineCommand
{
public:
	AddLineRendererComponent(const uint32_t& entity) :
		m_Entity(entity)
	{
	}
	void Execute(EngineCommand* edit)override;
	void Undo(EngineCommand* edit)override;
private:
	uint32_t m_Entity;
	uint32_t m_MapID;
};
// Rigidbody2Dコンポーネントを追加するコマンド
class AddRigidbody2DComponent :public IEngineCommand
{
public:
	AddRigidbody2DComponent(const uint32_t& entity) :
		m_Entity(entity)
	{
	}
	void Execute(EngineCommand* edit)override;
	void Undo(EngineCommand* edit)override;
private:
	uint32_t m_Entity;
};
// BoxCollider2Dコンポーネントを追加するコマンド
class AddBoxCollider2DComponent :public IEngineCommand
{
public:
	AddBoxCollider2DComponent(const uint32_t& entity) :
		m_Entity(entity)
	{
	}
	void Execute(EngineCommand* edit)override;
	void Undo(EngineCommand* edit)override;
private:
	uint32_t m_Entity;
};

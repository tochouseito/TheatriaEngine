#pragma once
#include <optional>
#include "EngineCommand/EngineCommand.h"
#include "Core/Utility/Components.h"
class GameObject;
class Prefab;
// 3Dオブジェクトを追加するコマンド
class Add3DObjectCommand :public IEngineCommand
{
public:
	Add3DObjectCommand()
	{
	}
	bool Execute(EngineCommand* edit)override;
	bool Undo(EngineCommand* edit)override;
	// オブジェクトのIDを取得
	uint32_t GetObjectID() const { return m_ObjectID; }
private:
	uint32_t m_ObjectID;
};
// カメラオブジェクトを追加するコマンド
class AddCameraObjectCommand :public IEngineCommand
{
public:
	AddCameraObjectCommand()
	{
	}
	bool Execute(EngineCommand* edit)override;
	bool Undo(EngineCommand* edit)override;
private:
	uint32_t m_ObjectID;
};
// パーティクルシステムオブジェクトを追加するコマンド
class AddParticleSystemObjectCommand :public IEngineCommand
{
public:
	AddParticleSystemObjectCommand()
	{
	}
	bool Execute(EngineCommand* edit)override;
	bool Undo(EngineCommand* edit)override;
private:
	uint32_t m_ObjectID;
};
// Effectオブジェクトを追加するコマンド
class AddEffectObjectCommand :public IEngineCommand
{
public:
	AddEffectObjectCommand()
	{
	}
	bool Execute(EngineCommand* edit)override;
	bool Undo(EngineCommand* edit)override;
private:
	uint32_t m_Entity;
};
// UIオブジェクトを追加するコマンド
class AddUIObjectCommand :public IEngineCommand
{
public:
	AddUIObjectCommand()
	{
	}
	bool Execute(EngineCommand* edit)override;
	bool Undo(EngineCommand* edit)override;
private:
	uint32_t m_ObjectID;
};
// LightObjectを追加するコマンド
class AddLightObjectCommand :public IEngineCommand
{
public:
	AddLightObjectCommand()
	{
	}
	bool Execute(EngineCommand* edit)override;
	bool Undo(EngineCommand* edit)override;
private:
	uint32_t m_ObjectID;
};
// MeshFilterComponentを追加するコマンド
class AddMeshFilterComponent :public IEngineCommand
{
public:
	AddMeshFilterComponent(const uint32_t& entity) :
		m_Entity(entity)
	{
	}
	bool Execute(EngineCommand* edit)override;
	bool Undo(EngineCommand* edit)override;
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
	bool Execute(EngineCommand* edit)override;
	bool Undo(EngineCommand* edit)override;
private:
	uint32_t m_Entity;
};
// MainCameraを設定するコマンド
class SetMainCamera :public IEngineCommand
{
public:
	SetMainCamera(const uint32_t& setCameraID):
		m_SetCameraID(setCameraID)
	{
	}
	bool Execute(EngineCommand* edit)override;
	bool Undo(EngineCommand* edit)override;
private:
	std::optional<uint32_t> m_PreCameraID = std::nullopt;
	std::optional<uint32_t> m_SetCameraID = std::nullopt;
};
// スクリプトコンポーネントを追加するコマンド
class AddScriptComponent :public IEngineCommand
{
public:
	AddScriptComponent(const uint32_t& entity,const uint32_t& objectID) :
		m_Entity(entity), m_ObjectID(objectID)
	{
	}
	bool Execute(EngineCommand* edit)override;
	bool Undo(EngineCommand* edit)override;
private:
	uint32_t m_Entity;
	uint32_t m_ObjectID;
};
// ラインレンダラーコンポーネントを追加するコマンド
class AddLineRendererComponent :public IEngineCommand
{
public:
	AddLineRendererComponent(const uint32_t& entity) :
		m_Entity(entity)
	{
	}
	bool Execute(EngineCommand* edit)override;
	bool Undo(EngineCommand* edit)override;
private:
	uint32_t m_Entity;
	uint32_t m_MapID;
};
// Rigidbody2Dコンポーネントを追加するコマンド
class AddRigidbody2DComponent :public IEngineCommand
{
public:
	AddRigidbody2DComponent(const uint32_t& entity,const uint32_t& objectID) :
		m_Entity(entity), m_ObjectID(objectID)
	{
	}
	bool Execute(EngineCommand* edit)override;
	bool Undo(EngineCommand* edit)override;
private:
	uint32_t m_Entity;
	uint32_t m_ObjectID;
};
// BoxCollider2Dコンポーネントを追加するコマンド
class AddBoxCollider2DComponent :public IEngineCommand
{
public:
	AddBoxCollider2DComponent(const uint32_t& entity) :
		m_Entity(entity)
	{
	}
	bool Execute(EngineCommand* edit)override;
	bool Undo(EngineCommand* edit)override;
private:
	uint32_t m_Entity;
};
// オブジェクトを削除するコマンド
class DeleteObjectCommand : public IEngineCommand
{
public:
	DeleteObjectCommand(const uint32_t& objectID) :
		m_ObjectID(objectID)
	{
	}
	bool Execute(EngineCommand* edit)override;
	bool Undo(EngineCommand* edit)override;
private:
	std::unique_ptr<Prefab> m_Prefab; // 削除前のPrefab情報を保持
	uint32_t m_ObjectID;
};
// オブジェクトの名前を変更するコマンド
class RenameObjectCommand : public IEngineCommand
{
public:
	RenameObjectCommand(const uint32_t& objectID, const std::wstring& name) :
		m_ObjectID(objectID), m_NewName(name)
	{
	}
	bool Execute(EngineCommand* edit)override;
	bool Undo(EngineCommand* edit)override;
private:
	uint32_t m_ObjectID;
	std::wstring m_NewName;
	std::wstring m_PreName;
};
// マテリアルコンポーネントを追加するコマンド
class AddMaterialComponent :public IEngineCommand
{
public:
	AddMaterialComponent(const uint32_t& entity) :
		m_Entity(entity)
	{
	}
	bool Execute(EngineCommand* edit)override;
	bool Undo(EngineCommand* edit)override;
private:
	uint32_t m_Entity;
};
// Emitterコンポーネントを追加するコマンド
class AddEmitterComponent :public IEngineCommand
{
public:
	AddEmitterComponent(const uint32_t& entity) :
		m_Entity(entity)
	{
	}
	bool Execute(EngineCommand* edit)override;
	bool Undo(EngineCommand* edit)override;
private:
	uint32_t m_Entity;
};
// Particleコンポーネントを追加するコマンド
class AddParticleComponent :public IEngineCommand
{
public:
	AddParticleComponent(const uint32_t& entity) :
		m_Entity(entity)
	{
	}
	bool Execute(EngineCommand* edit)override;
	bool Undo(EngineCommand* edit)override;
private:
	uint32_t m_Entity;
};
// 物理エンジンの重力を設定するコマンド
class SetGravityCommand : public IEngineCommand
{
public:
	SetGravityCommand(const Vector3& gravity) :
		m_Gravity(gravity)
	{
	}
	bool Execute(EngineCommand* edit)override;
	bool Undo(EngineCommand* edit)override;
private:
	Vector3 m_Gravity;
	Vector3 m_PreGravity;
};
// Audioコンポーネント追加
class AddAudioComponent :public IEngineCommand
{
public:
	AddAudioComponent(const uint32_t& entity) :
		m_Entity(entity)
	{
	}
	bool Execute(EngineCommand* edit)override;
	bool Undo(EngineCommand* edit)override;
private:
	uint32_t m_Entity;
};
// AnimationComponent追加
class AddAnimationComponent :public IEngineCommand
{
public:
	AddAnimationComponent(const uint32_t& entity) :
		m_Entity(entity)
	{
	}
	bool Execute(EngineCommand* edit)override;
	bool Undo(EngineCommand* edit)override;
private:
	uint32_t m_Entity;
};
// GameObjectを複製するコマンド
class CopyGameObjectCommand : public IEngineCommand
{
	public:
	CopyGameObjectCommand(const uint32_t& objectID) :
		m_ObjectID(objectID)
	{
	}
	bool Execute(EngineCommand* edit)override;
	bool Undo(EngineCommand* edit)override;
private:
	uint32_t m_ObjectID;
	uint32_t m_CopyObjectID;
};

// Effectを新規作成するコマンド
class CreateEffectCommand : public IEngineCommand
{
	public:
	CreateEffectCommand()
	{
	}
	bool Execute(EngineCommand* edit)override;
	bool Undo(EngineCommand* edit)override;
private:
	uint32_t m_Entity;
};

// EffectNodeを追加するコマンド
class AddEffectNodeCommand : public IEngineCommand
{
	public:
	AddEffectNodeCommand()
	{
	}
	bool Execute(EngineCommand* edit)override;
	bool Undo(EngineCommand* edit)override;
private:
	uint32_t m_Entity;
};

// Objectを複製するコマンド
class CloneObjectCommand : public IEngineCommand
{
public:
	CloneObjectCommand(const std::wstring& sceneName,const uint32_t& src)
		:m_CurrendSceneName(sceneName)
	{
	}
	bool Execute(EngineCommand* edit)override;
	bool Undo(EngineCommand* edit)override;
	// オブジェクトのIDを取得
	uint32_t GetObjectID() const { return m_DstID; }
private:
	uint32_t m_DstID;
	uint32_t m_SrcID;
	std::wstring m_CurrendSceneName;
};
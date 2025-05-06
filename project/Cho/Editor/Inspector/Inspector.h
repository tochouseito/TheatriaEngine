#pragma once
#include "Editor/BaseEditor/BaseEditor.h"
class GameObject;
class Inspector : public BaseEditor
{
public:
	Inspector(EditorManager* editorManager) :
		BaseEditor(editorManager)
	{
	}
	~Inspector()
	{
	}
	void Initialize() override;
	void Update() override;
	void Window() override;
private:
	void ComponentsView(GameObject* object);
	void TransformComponentView(GameObject* object);
	void MeshFilterComponentView(GameObject* object);
	void MeshRendererComponentView(GameObject* object);
	void CameraComponentView(GameObject* object);
	void MaterialComponentView(GameObject* object);
	void ScriptComponentView(GameObject* object);
	void LineRendererComponentView(GameObject* object);
	void Rigidbody2DComponentView(GameObject* object);
	void BoxCollider2DComponentView(GameObject* object);
	void EmitterComponentView(GameObject* object);
	void ParticleComponentView(GameObject* object);
	// コンポーネントの追加
	void AddComponent(GameObject* object);
};


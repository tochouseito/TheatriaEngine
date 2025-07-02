#include "pch.h"
#include "SceneManager.h"
#include "GameCore/GameWorld/GameWorld.h"


// デフォルトのシーンを作成
GameScene SceneManager::CreateDefaultScene()
{
	GameScene scene(L"MainScene");
	// デフォルトのObjectを追加
	// Cube
	CPrefab cube(L"Cube", ObjectType::MeshObject);
	TransformComponent transform;
	MeshFilterComponent meshFilter;
	meshFilter.modelName = L"Cube"; // モデル名を設定
	MeshRendererComponent meshRenderer;
	cube.AddComponent<TransformComponent>(transform);
	cube.AddComponent<MeshFilterComponent>(meshFilter);
	cube.AddComponent<MeshRendererComponent>(meshRenderer);
	// MainCamera
	CPrefab mainCamera(L"MainCamera", ObjectType::Camera);
	TransformComponent cameraTransform;
	cameraTransform.position.z = -20.0f;
	CameraComponent cameraComponent;
	mainCamera.AddComponent<TransformComponent>(cameraTransform);
	mainCamera.AddComponent<CameraComponent>(cameraComponent);
	// DirectionalLight
	CPrefab directionalLight(L"DirectionalLight", ObjectType::Light);
	TransformComponent lightTransform;
	lightTransform.degrees.x = 45.0f;
	lightTransform.degrees.y = 45.0f;
	LightComponent lightComponent;
	lightComponent.type = LightType::Directional;
	directionalLight.AddComponent<TransformComponent>(lightTransform);
	directionalLight.AddComponent<LightComponent>(lightComponent);
	// シーンに追加
	scene.AddPrefab(cube);
	scene.AddPrefab(mainCamera);
	scene.AddPrefab(directionalLight);
	// mainCameraを設定
	scene.SetStartCameraName(L"MainCamera");
	AddScene(scene);
	LoadScene(L"MainScene",true);
	return m_Scenes[0]; // 最初のシーンを返す
}

// 読み込まれているシーンを破棄して指定したシーンをロード
GameSceneInstance* SceneManager::LoadScene(const std::wstring& sceneName, const bool& updateMaincamera)
{
	// コンテナ
	if (!m_SceneNameToID.contains(sceneName))
	{
		return nullptr; // シーンが存在しない場合はnullptrを返す
	}
	// 読み込まれているシーンをすべて破棄
	m_pGameWorld->ClearAllScenes(); // ゲームワールドのシーンをクリア
	// シーンインスタンスを破棄
	m_pSceneInstances.clear();
	// ロード
	SceneID id = m_pGameWorld->AddGameObjectFromScene(m_Scenes[m_SceneNameToID[sceneName]],updateMaincamera);
	// シーンインスタンスを作成
	m_pSceneInstances.push_back(std::make_unique<GameSceneInstance>(this, id));
	// シーンインスタンスを返す
	return m_pSceneInstances.back().get();
}

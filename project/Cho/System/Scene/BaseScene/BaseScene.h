#pragma once
#include<string>
class SceneManager;
class BaseScene
{
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~BaseScene() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize()=0;

	/// <summary>
	/// 終了
	/// </summary>
	virtual void Finalize() = 0;

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// シーンの切り替え依頼
	/// </summary>
	virtual void ChangeScene()=0;

	virtual void SetSceneManager(SceneManager* sceneManager) { sceneManager_ = sceneManager; }

	virtual SceneManager* GetSceneManager()const { return sceneManager_; }

	std::string CreateGameObject(const std::string& name);

	std::string CreateCamera(const std::string& name);
private:
	/*シーンマネージャを借りてくる*/
	SceneManager* sceneManager_ = nullptr;
};

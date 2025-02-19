#include "PrecompiledHeader.h"
#include "SceneFactory.h"
#include "Scene/MainScene.h"
#include "Game/Scenes/GameScene/GameScene.h"
#include <cassert>

BaseScene* SceneFactory::CreateScene(const std::string& sceneName)
{
	/*次のシーン生成*/
	BaseScene* newScene = nullptr;
	if (sceneName == "MainScene") {
		newScene = new MainScene();
	}
	else if (sceneName == "GameScene") {
		newScene = new GameScene();
	}
	else {
		// 未定義のシーン名
		assert(0);
	}
	return newScene;
}

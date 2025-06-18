#pragma once
#include "Engine/Engine.h"
#include "APIExportsMacro.h"
#include "Externals/ChoMath/ChoMath.h"
#include <variant>
#include <optional>
using GameParameterVariant = std::variant<int, float, bool, Vector3>;
class GameObject;
class ChoEngine;
namespace Cho
{
	// エディタとゲーム実行ファイルしか許可しない
#ifdef ENGINECREATE_FUNCTION
	// Engineの生成
	CHO_API Engine* CreateEngine(RuntimeMode mode);
	// Engineの破棄
	CHO_API void DestroyEngine(Engine* engine);
	// ポインタを受け取る
	CHO_API void SetEngine(Engine* engine);
	// Engineのポインタ
	static ChoEngine* g_Engine = nullptr;
#endif
}
namespace ChoSystem
{
	// スクリプト用
#ifdef USE_CHOENGINE_SCRIPT
	// Json保存
	CHO_API bool SaveGameParameter(const std::wstring& filePath,
		const std::string& group,
		const std::string& item,
		const std::string& dataName,
		const GameParameterVariant& value);
	// Json読み込み
	CHO_API bool LoadGameParameter(const std::wstring& filePath,
		const std::string& group,
		const std::string& item,
		const std::string& dataName,
		GameParameterVariant& outValue);
	// ゲームオブジェクト取得
	CHO_API GameObject* FindGameObjectByName(std::wstring_view name);
	// ゲームオブジェクト複製
	CHO_API GameObject* CloneGameObject(const GameObject* srcObj, Vector3 generatePosition);
	// ゲームオブジェクト削除
	CHO_API void DestroyGameObject(std::optional<uint32_t> id);
	// デルタタイム取得
	CHO_API float DeltaTime();

	// SceneManager
	struct SceneManagerAPI
	{
		CHO_API void LoadScene(const std::wstring& sceneName);
		CHO_API void UnloadScene(const std::wstring& sceneName);
		CHO_API void ChangeMainScene(const std::wstring& sceneName);
	};
	CHO_API extern SceneManagerAPI sceneManager;
#endif
}
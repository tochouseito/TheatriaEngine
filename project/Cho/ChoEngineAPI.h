#pragma once
#include "Engine/Engine.h"
#include "APIExportsMacro.h"
#include "Externals/ChoMath/ChoMath.h"
#include <variant>
using GameParameterVariant = std::variant<int, float, bool, Vector3>;
//struct ScriptContext;
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
	// ゲームオブジェクト複製
	CHO_API void CloneGameObject(std::optional<uint32_t> id, Vector3 generatePosition);
	// ゲームオブジェクト削除
	CHO_API void DestroyGameObject(std::optional<uint32_t> id);
#endif
}

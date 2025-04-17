#pragma once
#include "Engine/Engine.h"
#ifdef CHOENGINE_EXPORTS
#define CHO_API __declspec(dllexport)
#else
#define CHO_API __declspec(dllimport)
#endif

namespace Cho
{
    // Engineの生成
	CHO_API Engine* CreateEngine(RuntimeMode mode);
	// Engineの破棄
	CHO_API void DestroyEngine(Engine* engine);
}

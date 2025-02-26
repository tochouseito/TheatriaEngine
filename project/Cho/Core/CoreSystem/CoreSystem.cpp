#include "pch.h"
#include "CoreSystem.h"

CoreSystem::CoreSystem()
{
}

CoreSystem::~CoreSystem()
{
}

void CoreSystem::Initialize()
{
	// スレッドマネージャー
	threadManager = std::make_unique<ThreadManager>();
	//threadManager->Initialize();
}

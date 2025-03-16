#include "pch.h"
#include "CoreSystem.h"

CoreSystem::CoreSystem()
{
	Initialize();
}

CoreSystem::~CoreSystem()
{
}

void CoreSystem::Initialize()
{
	// スレッドマネージャー
	ThreadManager::GetInstance().Initialize();
}

void CoreSystem::Finalize()
{
	ThreadManager::GetInstance().StopAllThreads();
}

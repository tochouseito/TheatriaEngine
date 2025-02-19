#include "PrecompiledHeader.h"
#include "ChoEngine.h"

void ChoEngine::Initialize()
{
}

void ChoEngine::Finalize()
{
}

void ChoEngine::Operation()
{
	/*初期化*/
	Initialize();

	/*メインループ*/
	while (true) {
		if (winApp->ProcessMessage()) {
			break;
		}
	}

	/*終了処理*/
	Finalize();
}

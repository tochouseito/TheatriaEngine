#pragma warning(push)
#pragma warning(disable:28251)

// Engine
#include "Cho/Main/ChoEngine.h"

// Windows
#include <Windows.h>

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#pragma warning(pop)

	std::unique_ptr<Engine> engine = std::make_unique<ChoEngine>();
	engine->Operation();

	return 0;
}
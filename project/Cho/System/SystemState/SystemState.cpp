#include "SystemState.h"

void SystemState::Initialize()
{

}

bool SystemState::WindowResize()
{
	if (preWindowHeight != windowHeight ||
		preWindowWidth != windowWidth) {
		preWindowHeight = windowHeight;
		preWindowWidth = windowWidth;
		return true;
	}
	preWindowHeight = windowHeight;
	preWindowWidth = windowWidth;
	return false;
}
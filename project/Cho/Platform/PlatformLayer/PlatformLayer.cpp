#include "pch.h"
#include "PlatformLayer.h"

PlatformLayer::PlatformLayer()
{
	Initialize();
}

PlatformLayer::~PlatformLayer()
{
}

void PlatformLayer::Initialize()
{
	timer = std::make_unique<Timer>();
}

void PlatformLayer::Finalize()
{

}

void PlatformLayer::Update()
{
	timer->Update();
}

void PlatformLayer::StartFrame()
{
	timer->Start();
}

void PlatformLayer::EndFrame()
{
	timer->End();
}

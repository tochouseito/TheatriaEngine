#include "PrecompiledHeader.h"
#include "GraphicsSystem.h"
#include"D3D12/D3DDevice/D3DDevice.h"

void GraphicsSystem::Initialize(D3DDevice* d3dDevice)
{
	// シェーダコンパイラーの初期化
	shaderCompiler = std::make_unique<ShaderCompiler>();
	shaderCompiler->Initialize(d3dDevice);

	// グラフィックパイプラインの初期化
	pipeline = std::make_unique<Pipeline>();
	pipeline->Initialize(d3dDevice,shaderCompiler.get());

}

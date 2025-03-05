#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN // ヘッダーから余計な情報を除外
#endif // !WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdint.h>
#include <memory>

#include <d3d12.h>
#include <dxgi1_6.h>

#ifndef D3D12_GPU_VIRTUAL_ADDRESS_NULL
#define D3D12_GPU_VIRTUAL_ADDRESS_NULL ((D3D12_GPU_VIRTUAL_ADDRESS)0)
#endif

#ifndef PixelFormat
#define PixelFormat (DXGI_FORMAT_R8G8B8A8_UNORM)
#endif

#include <wrl.h>
using Microsoft::WRL::ComPtr;

#include "Core/Log/Log.h"

// FreeListContainer
#include "Core/Utility/FArray.h"
#include "Core/Utility/FVector.h"
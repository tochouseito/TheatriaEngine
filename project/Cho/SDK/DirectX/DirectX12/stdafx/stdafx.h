#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN // ヘッダーから余計な情報を除外
#endif // !WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <d3d12.h>
#include <dxgi1_6.h>

#include <wrl.h>
using Microsoft::WRL::ComPtr;

#include "Core/Log/Log.h"
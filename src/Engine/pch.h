#ifndef PCH_H
#define PCH_H

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

// Link necessary d3d12 libraries.
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

#define DEEP_PROFILER
#define ENTITY_BASE_MAXIMUM 1024;
#define FIXED_DT 0.008333f

#include <vector>
#include <list>
#include <queue>
#include <unordered_map>

#include <Windows.h>
#include <timeapi.h>
#include "libs/d3dUtils.h"
#include <DirectXMath.h>
#include "RenderWindow.h"

struct TRANSFORM;

#include "Transform.h"
#include "GameTimer.h"

#endif
#pragma once

// libs
#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "d3dcompiler")

#ifdef _DEBUG
#pragma comment(lib, "DirectXTK\\DirectXTK_debug.lib")
#else
#pragma comment(lib, "DirectXTK\\DirectXTK.lib")
#endif

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex\\DirectXTex_debug.lib")
#else
#pragma comment(lib, "DirectXTex\\DirectXTex.lib")
#endif

#ifdef _DEBUG
#pragma comment(lib, "FBX\\debug\\libfbxsdk-md.lib")
#pragma comment(lib, "FBX\\debug\\libxml2-md.lib")
#pragma comment(lib, "FBX\\debug\\zlib-md.lib")
#else
#pragma comment(lib, "FBX\\release\\libfbxsdk-md.lib")
#pragma comment(lib, "FBX\\release\\libxml2-md.lib")
#pragma comment(lib, "FBX\\release\\zlib-md.lib")
#endif

#ifdef _DEBUG
#pragma comment(lib, "PixEvents\\debug\\WinPixEventRuntime.lib")
#else
#pragma comment(lib, "PixEvents\\release\\WinPixEventRuntime.lib")
#endif

#pragma comment(lib, "DXC\\dxcompiler.lib")
#pragma comment(lib, "DXC\\dxil.lib")

#define _HAS_STD_BYTE 0

// includes
#include <windows.h>
#include <tchar.h>
#include <memory>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <array>
#include <list>
#include <set>
#include <map>
#include <functional>
#include <variant>
using namespace std;

#include <filesystem>
namespace fs = std::filesystem;

#include "d3dx12.h"
#include "DirectXTK/SimpleMath.h"
#include <d3d12.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace Microsoft::WRL;

#include <DXC/dxcapi.h>

#include <DirectXTex/DirectXTex.h>
#include <DirectXTex/DirectXTex.inl>
#include "FBX/fbxsdk.h"
namespace fbx = fbxsdk;

#include "PixEvents/pix3.h"
#include "PixEvents/pix3_win.h"
#include "PixEvents/PIXEvents.h"
#include "PixEvents/PIXEventsCommon.h"

#include "Utils.h"
#include "Defs.h"

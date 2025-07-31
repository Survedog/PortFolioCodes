#include "pch.h"
#include "DxcManager.h"

void DxcManager::Init()
{
	DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&_dxcUtils));
	DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&_dxcCompiler));
	assert(_dxcUtils != nullptr && _dxcCompiler != nullptr);

	_dxcUtils->CreateDefaultIncludeHandler(&_dxcIncludeHandler);
	assert(_dxcIncludeHandler != nullptr);
}

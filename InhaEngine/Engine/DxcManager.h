#pragma once
class DxcManager
{
public:
	void Init();

	ComPtr<IDxcUtils> GetDxcUtils() { return _dxcUtils; }
	ComPtr<IDxcCompiler3> GetDxcCompiler() { return _dxcCompiler; }
	ComPtr<IDxcIncludeHandler> GetDxcIncludeHandler() { return _dxcIncludeHandler; }

private:
	ComPtr<IDxcUtils> _dxcUtils = nullptr;
	ComPtr<IDxcCompiler3> _dxcCompiler = nullptr;
	ComPtr<IDxcIncludeHandler> _dxcIncludeHandler = nullptr;
};


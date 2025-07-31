#pragma once

class Texture;
class RenderTargetGroup;

class SwapChainManager
{
public:
	SwapChainManager();

	void Init(const WindowInfo& windowInfo, ComPtr<ID3D12Device> device, ComPtr<IDXGIFactory6> dxgiFactory, ComPtr<ID3D12CommandQueue> cmdQueue);
	void Present();
	void SwapBuffer();

	inline ComPtr<IDXGISwapChain1> GetSwapChain() { return _swapChain; }
	inline uint32 GetCurrentBackBufferIdx() { return _backBufferIdx; }
	inline bool IsMSAAEnabled() const { return _4xMSAAState; }
	inline uint32 GetMSAASampleCount() const { return _4xMSAAState ? 4 : 1; }
	inline uint32 GetMSAAQualityLevelCount() const { return _numMSAAQualityLevels; }

private:
	ComPtr<IDXGISwapChain1> _swapChain;
	uint32 _backBufferCount;
	uint32 _backBufferIdx;

	uint32 _clientWidth;
	uint32 _clientHeight;
	bool _4xMSAAState;
	uint32 _numMSAAQualityLevels;
};


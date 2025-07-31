#include "pch.h"
#include "SwapChainManager.h"
#include "Engine.h"
#include "CommandQueueManager.h"
#include "RenderTargetGroup.h"

SwapChainManager::SwapChainManager() :
    _swapChain(nullptr),
    _backBufferCount(SWAP_CHAIN_BUFFER_COUNT),
    _backBufferIdx(0),
    _clientWidth(800),
    _clientHeight(600),
    _4xMSAAState(true),
    _numMSAAQualityLevels(1)
{}

void SwapChainManager::Init(const WindowInfo& windowInfo, ComPtr<ID3D12Device> device, ComPtr<IDXGIFactory6> dxgiFactory, ComPtr<ID3D12CommandQueue> cmdQueue)
{   
    _clientWidth = windowInfo.width;
    _clientHeight = windowInfo.height;

    // Check hardware support
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
    msQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    msQualityLevels.SampleCount = GetMSAASampleCount();
    msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    msQualityLevels.NumQualityLevels = 0;
    THROW_DX_IF_FAILED(device->CheckFeatureSupport(
        D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
        &msQualityLevels,
        sizeof(msQualityLevels)));

    _numMSAAQualityLevels = msQualityLevels.NumQualityLevels;
    if (_4xMSAAState) assert(_numMSAAQualityLevels > 0 && "Device doesn't support 4xMSAA.");

    // Create swap chain
    DXGI_SWAP_CHAIN_DESC1 scDesc;
    scDesc.Width = _clientWidth;
    scDesc.Height = _clientHeight;
    scDesc.Format = msQualityLevels.Format;
    scDesc.Stereo = FALSE;
    scDesc.SampleDesc.Count = 1;
    scDesc.SampleDesc.Quality = 0;
    scDesc.BufferCount = _backBufferCount;
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scDesc.Scaling = DXGI_SCALING_STRETCH;
    scDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC scFullScreenDesc;
    scFullScreenDesc.RefreshRate.Numerator = 60;
    scFullScreenDesc.RefreshRate.Denominator = 1;
    scFullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    scFullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    scFullScreenDesc.Windowed = windowInfo.windowed;
  
    THROW_DX_IF_FAILED(dxgiFactory->CreateSwapChainForHwnd(cmdQueue.Get(), windowInfo.hwnd, &scDesc, &scFullScreenDesc, nullptr, &_swapChain));
}

void SwapChainManager::Present()
{
    _swapChain->Present(0, 0);
}

void SwapChainManager::SwapBuffer()
{
    _backBufferIdx = (_backBufferIdx + 1) % _backBufferCount;
}

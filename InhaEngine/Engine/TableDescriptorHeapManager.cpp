#include "pch.h"
#include "TableDescriptorHeapManager.h"
#include "Engine.h"
#include "DeviceManager.h"
#include "CommandQueueManager.h"

void GraphicsDescriptorHeapManager::Init(uint32 maxDescCount)
{
	ComPtr<ID3D12Device> device = ENGINE->GetDeviceManager()->GetDevice();
	assert(device);

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = maxDescCount;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_descHeap));

	_handleSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	_currentTableType = DESC_TABLE_TYPE::NONE;
}

void GraphicsDescriptorHeapManager::Clear()
{
	_tableStartOffset = 0;
}

void GraphicsDescriptorHeapManager::SetDesc(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, DESC_TABLE_TYPE tableType, DescTableRegister reg)
{
	assert(tableType == _currentTableType && L"The input descriptor table type is different from the one that's currently being built.");

	D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetCPUHandle(static_cast<uint32>(reg.value));
	uint32 destRange = 1;
	uint32 srcRange = 1;
	ComPtr<ID3D12Device> device = ENGINE->GetDeviceManager()->GetDevice();
	device->CopyDescriptors(1, &destHandle, &destRange, 1, &srcHandle, &srcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void GraphicsDescriptorHeapManager::CommitTable(uint32 rootParamIdx, uint32 tableDescCount)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle = _descHeap->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += _tableStartOffset * _handleSize;
	ENGINE->GetCommandQueueManager()->GetGraphicsCommandList()->SetGraphicsRootDescriptorTable(rootParamIdx, handle);

	_tableStartOffset += tableDescCount;
	_currentTableType = DESC_TABLE_TYPE::NONE;
}

D3D12_CPU_DESCRIPTOR_HANDLE GraphicsDescriptorHeapManager::GetCPUHandle(uint32 cbvOffset)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = _descHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += (_tableStartOffset + cbvOffset) * _handleSize;
	return handle;
}

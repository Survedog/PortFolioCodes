#pragma once

class GraphicsDescriptorHeapManager
{
public:
	void Init(uint32 maxDescCount);

public:
	void Clear();
	
	ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() { return _descHeap; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint32 cbvOffset);
	
	inline void StartBuildingTable(DESC_TABLE_TYPE tableType) { _currentTableType = tableType; }
	inline void CancelBuildingTable() { _currentTableType = DESC_TABLE_TYPE::NONE; }

	void SetDesc(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, DESC_TABLE_TYPE tableType, DescTableRegister reg);
	void CommitTable(uint32 rootParamIdx, uint32 tableDescCount);

private:
	ComPtr<ID3D12DescriptorHeap> _descHeap;

	uint64 _handleSize = 0;
	uint32 _tableStartOffset = 0;

	DESC_TABLE_TYPE _currentTableType = DESC_TABLE_TYPE::NONE;
};
#pragma once
class RootSignatureManager
{
public:
	void Init();

	inline ComPtr<ID3D12RootSignature> GetGraphicsRootSignature() { return _graphicsRootSignature; }

private:
	void CreateGraphicsRootSignature();
	vector<D3D12_STATIC_SAMPLER_DESC> GetStaticSamplers() const;

private:
	ComPtr<ID3D12RootSignature> _graphicsRootSignature;
};


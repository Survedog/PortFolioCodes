#pragma once

#include "Object.h"

enum class SHADER_TYPE : uint8
{
	//DEFERRED,
	FORWARD,
	//LIGHTING,
	//PARTICLE,
	//COMPUTE,
	SHADOW,
	END
};

enum class RASTERIZER_TYPE : uint8
{
	CULL_NONE,
	CULL_FRONT,
	CULL_BACK,
	WIREFRAME,
};

enum class DEPTH_STENCIL_TYPE : uint8
{
	LESS,
	LESS_EQUAL,
	GREATER,
	GREATER_EQUAL,
	NO_DEPTH_TEST, // Depth Test(X) + Depth Write(O)
	NO_DEPTH_TEST_NO_WRITE, // Depth Test(X) + Depth Write(X)
	LESS_NO_WRITE, // Depth Test(O) + Depth Write(X)
	LESS_EQUAL_NO_WRITE, // Depth Test(O) + Depth Write(X)
};

enum class BLEND_TYPE : uint8
{
	DEFAULT,
	ALPHA_BLEND,
	ONE_TO_ONE_BLEND,
	END,
};

enum
{
	SHADER_TYPE_COUNT = static_cast<uint8>(SHADER_TYPE::END)
};

struct ShaderInfo
{
	SHADER_TYPE shaderType = SHADER_TYPE::FORWARD;
	RASTERIZER_TYPE rasterizerType = RASTERIZER_TYPE::CULL_BACK;
	DEPTH_STENCIL_TYPE depthStencilType = DEPTH_STENCIL_TYPE::LESS;
	BLEND_TYPE blendType = BLEND_TYPE::DEFAULT;
	D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	DXGI_SAMPLE_DESC sampleDesc = {};
};

// NOTE: vsName and psName must be set. (Others are optional. psName also can be optional when writing depth is the only purpose.)
struct ShaderArgument
{
	wstring vsName = L"VS";
	wstring hsName;
	wstring dsName;
	wstring gsName;
	wstring psName = L"PS";

	vector<DxcDefine> vsDefines;
	vector<DxcDefine> hsDefines;
	vector<DxcDefine> dsDefines;
	vector<DxcDefine> gsDefines;
	vector<DxcDefine> psDefines;
};

class Shader : public Object
{
public:
	Shader();
	virtual ~Shader() = default;

public:
	void Create(const wstring& filePath, ShaderInfo info, const ShaderArgument& arg, const vector<D3D12_INPUT_ELEMENT_DESC>& inputElementDescriptors);

	void ApplyShader(ComPtr<ID3D12GraphicsCommandList> commandList);

	inline ShaderInfo GetShaderInfo() const { return _info; }

private:
	void CreateGraphicsShaders(const wstring& filePath, ShaderInfo info, const ShaderArgument& arg);
	void CreatePSO(ShaderInfo info, const ShaderArgument& arg, const vector<D3D12_INPUT_ELEMENT_DESC>& inputElementDescriptors);

	void CreateShader(const wstring& filePath, const wstring& name, const wstring& version, ComPtr<IDxcBlob>& errorBlob, D3D12_SHADER_BYTECODE& shaderByteCode, const vector<DxcDefine>& defines);

	inline void CreateVertexShader(const wstring& filePath, const wstring& name, const wstring& version, const vector<DxcDefine>& defines)
	{
		CreateShader(filePath, name, version, _vsBlob, _psoDesc.VS, defines);
	}

	inline void CreateHullShader(const wstring& filePath, const wstring& name, const wstring& version, const vector<DxcDefine>& defines)
	{
		CreateShader(filePath, name, version, _hsBlob, _psoDesc.HS, defines);
	}

	inline void CreateDomainShader(const wstring& filePath, const wstring& name, const wstring& version, const vector<DxcDefine>& defines)
	{
		CreateShader(filePath, name, version, _dsBlob, _psoDesc.DS, defines);
	}

	inline void CreateGeometryShader(const wstring& filePath, const wstring& name, const wstring& version, const vector<DxcDefine>& defines)
	{
		CreateShader(filePath, name, version, _gsBlob, _psoDesc.GS, defines);
	}

	inline void CreatePixelShader(const wstring& filePath, const wstring& name, const wstring& version, const vector<DxcDefine>& defines)
	{
		CreateShader(filePath, name, version, _psBlob, _psoDesc.PS, defines);
	}

	D3D12_PRIMITIVE_TOPOLOGY_TYPE GetTopologyType(D3D12_PRIMITIVE_TOPOLOGY topology) const;

	void ApplyPSOShaderType(SHADER_TYPE type);
	void ApplyPSORasterizerType(RASTERIZER_TYPE type);
	void ApplyPSODepthStencilType(DEPTH_STENCIL_TYPE type);
	void ApplyPSOBlendType(BLEND_TYPE type);

private:
	ShaderInfo _info = {};
	D3D12_GRAPHICS_PIPELINE_STATE_DESC _psoDesc = {};
	ComPtr<ID3D12PipelineState> _pso;

	ComPtr<IDxcBlob> _vsBlob;
	ComPtr<IDxcBlob> _hsBlob;
	ComPtr<IDxcBlob> _dsBlob;
	ComPtr<IDxcBlob> _gsBlob;
	ComPtr<IDxcBlob> _psBlob;
	ComPtr<IDxcBlobEncoding> _errorBlob;	
};


#include "pch.h"
#include "Shader.h"
#include "Engine.h"
#include "DeviceManager.h"
#include "RootSignatureManager.h"
#include "SwapChainManager.h"
#include "DxcManager.h"

Shader::Shader() :
	Object(OBJECT_TYPE::SHADER)
{
}

void Shader::Create(const wstring& filePath, ShaderInfo info, const ShaderArgument& arg, const vector<D3D12_INPUT_ELEMENT_DESC>& inputElementDescriptors)
{
	CreateGraphicsShaders(filePath, info, arg);
	CreatePSO(info, arg, inputElementDescriptors);
}

void Shader::ApplyShader(ComPtr<ID3D12GraphicsCommandList> commandList)
{
	assert(commandList != nullptr);
	commandList->IASetPrimitiveTopology(_info.topology);
	commandList->SetPipelineState(_pso.Get());
}

void Shader::CreateGraphicsShaders(const wstring& filePath, ShaderInfo info, const ShaderArgument& arg)
{
	_info = info;

	CreateVertexShader(filePath, arg.vsName, L"vs_6_0", arg.vsDefines);
	
	if (!arg.psName.empty()) CreatePixelShader(filePath, arg.psName, L"ps_6_0", arg.psDefines);
	if (!arg.hsName.empty()) CreateHullShader(filePath, arg.hsName, L"hs_6_0", arg.hsDefines);
	if (!arg.dsName.empty()) CreateDomainShader(filePath, arg.dsName, L"ds_6_0", arg.dsDefines);
	if (!arg.gsName.empty()) CreateGeometryShader(filePath, arg.gsName, L"gs_6_0", arg.gsDefines);
}

void Shader::CreatePSO(ShaderInfo info, const ShaderArgument& arg, const vector<D3D12_INPUT_ELEMENT_DESC>& inputElementDescriptors)
{
	_psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	_psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	_psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	_psoDesc.InputLayout = { inputElementDescriptors.data(), static_cast<UINT>(inputElementDescriptors.size())};
	_psoDesc.pRootSignature = ENGINE->GetRootSignatureManager()->GetGraphicsRootSignature().Get();	

	ApplyPSOShaderType(info.shaderType);
	ApplyPSOBlendType(info.blendType);
	ApplyPSORasterizerType(info.rasterizerType);
	ApplyPSODepthStencilType(info.depthStencilType);

	_psoDesc.SampleDesc = info.sampleDesc;
	_psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	_psoDesc.SampleMask = UINT_MAX;
	_psoDesc.PrimitiveTopologyType = GetTopologyType(info.topology);

	ComPtr<ID3D12Device> device = ENGINE->GetDeviceManager()->GetDevice();
	HRESULT hr = device->CreateGraphicsPipelineState(&_psoDesc, IID_PPV_ARGS(&_pso));
	THROW_MSG_DX_IF_FAILED(hr, L"Failed to create Pipeline State Object.");
}

void Shader::CreateShader(const wstring& filePath, const wstring& name, const wstring& version, ComPtr<IDxcBlob>& blob, D3D12_SHADER_BYTECODE& shaderByteCode, const vector<DxcDefine>& defines)
{
	uint32 compileFlag = 0;
#ifdef _DEBUG
	compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	// Construct arguments for dxc compilation.
	ComPtr<IDxcBlobEncoding> sourceBlob = nullptr;
	uint32 codePage = CP_UTF8;
	ENGINE->GetDxcManager()->GetDxcUtils()->LoadFile(filePath.c_str(), &codePage, &sourceBlob);	
	DxcBuffer sourceBuffer = { sourceBlob->GetBufferPointer(), sourceBlob->GetBufferSize(), codePage };

	wstring outputFileName = L"ShaderOutput_";
	outputFileName.append(name);
	outputFileName.append(L".dxil");

	wstring shaderDirectory = fs::absolute(fs::path(filePath).parent_path()).wstring();	

	vector<LPCWSTR> argumentsInfo = {
		L"-I", shaderDirectory.c_str(),
		L"-Fo", outputFileName.c_str(),
		L"-Zi",
	};

	IDxcCompilerArgs* compilerArgs;
	ENGINE->GetDxcManager()->GetDxcUtils()->BuildArguments(
		fs::path(filePath).filename().c_str(),
		name.c_str(),
		version.c_str(),
		argumentsInfo.data(),
		static_cast<uint32>(argumentsInfo.size()),
		defines.data(),
		static_cast<uint32>(defines.size()),
		&compilerArgs);	

	// Compile shader.
	ComPtr<IDxcResult> compileResult = nullptr;
	ComPtr<IDxcIncludeHandler> includeHandler = ENGINE->GetDxcManager()->GetDxcIncludeHandler();
	ENGINE->GetDxcManager()->GetDxcCompiler()->Compile(&sourceBuffer, compilerArgs->GetArguments(), compilerArgs->GetCount(), includeHandler.Get(), IID_PPV_ARGS(&compileResult));

	// Save shader byte code if compilation has succeeded.
	HRESULT hr;
	compileResult->GetStatus(&hr);
	compileResult->GetErrorBuffer(&_errorBlob);
	THROW_MSG_DX_IF_FAILED(hr, L"Failed to create shader.\n" + AnsiToWString(reinterpret_cast<char*>(_errorBlob->GetBufferPointer())));	

	compileResult->GetResult(blob.GetAddressOf());
	shaderByteCode.BytecodeLength = blob->GetBufferSize();
	shaderByteCode.pShaderBytecode = blob->GetBufferPointer();

	// Create shader PDB file.
	ComPtr<IDxcBlob> pdbBlob = nullptr;
	ComPtr<IDxcBlobUtf16> pdbNameBlob = nullptr;
	if (SUCCEEDED(compileResult->GetOutput(DXC_OUT_PDB, IID_PPV_ARGS(&pdbBlob), &pdbNameBlob)))
	{
		wstring pdbOutputPath = shaderDirectory; // pdb file name will be automatically generated using hash. (Use this so that PIX can find it quickly)
		pdbOutputPath.append(L"\\pdbs\\");
		pdbOutputPath.append(pdbNameBlob->GetStringPointer());

		FILE* fp = NULL;
		if (_wfopen_s(&fp, pdbOutputPath.c_str(), L"wb") == 0) // File open succeeded. 
		{
			wstring name = pdbNameBlob->GetStringPointer();
			fwrite(pdbBlob->GetBufferPointer(), pdbBlob->GetBufferSize(), 1, fp);
			fclose(fp);
		}
	}
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE Shader::GetTopologyType(D3D12_PRIMITIVE_TOPOLOGY topology) const
{
	switch (topology)
	{
	case D3D_PRIMITIVE_TOPOLOGY_POINTLIST:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	case D3D_PRIMITIVE_TOPOLOGY_LINELIST:
	case D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
	case D3D_PRIMITIVE_TOPOLOGY_LINESTRIP:
	case D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	case D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
	default:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
	}
}

void Shader::ApplyPSOShaderType(SHADER_TYPE type)
{
	switch (type)
	{
	case SHADER_TYPE::SHADOW:
		_psoDesc.NumRenderTargets = 0;
		_psoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
		_psoDesc.RasterizerState.DepthBias = 10000;
		_psoDesc.RasterizerState.DepthBiasClamp = 0.f;
		_psoDesc.RasterizerState.SlopeScaledDepthBias = 1.f;
		break;
	case SHADER_TYPE::FORWARD:
	default:
		_psoDesc.NumRenderTargets = 1;
		_psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		break;
	}
}

void Shader::ApplyPSORasterizerType(RASTERIZER_TYPE type)
{
	switch (type)
	{
	case RASTERIZER_TYPE::CULL_BACK:
		_psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
		_psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		break;
	case RASTERIZER_TYPE::CULL_FRONT:
		_psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
		_psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;		
		break;
	case RASTERIZER_TYPE::CULL_NONE:
		_psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		_psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		break;
	case RASTERIZER_TYPE::WIREFRAME:
		_psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		_psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
		break;
	}
}

void Shader::ApplyPSODepthStencilType(DEPTH_STENCIL_TYPE type)
{
	switch (type)
	{
	case DEPTH_STENCIL_TYPE::LESS:
		_psoDesc.DepthStencilState.DepthEnable = TRUE;
		_psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		break;
	case DEPTH_STENCIL_TYPE::LESS_NO_WRITE:
		_psoDesc.DepthStencilState.DepthEnable = TRUE;
		_psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		_psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		break;
	case DEPTH_STENCIL_TYPE::LESS_EQUAL:
		_psoDesc.DepthStencilState.DepthEnable = TRUE;
		_psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		break;
	case DEPTH_STENCIL_TYPE::LESS_EQUAL_NO_WRITE:
		_psoDesc.DepthStencilState.DepthEnable = TRUE;
		_psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		_psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		break;
	case DEPTH_STENCIL_TYPE::GREATER:
		_psoDesc.DepthStencilState.DepthEnable = TRUE;
		_psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;
		break;
	case DEPTH_STENCIL_TYPE::GREATER_EQUAL:
		_psoDesc.DepthStencilState.DepthEnable = TRUE;
		_psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		break;
	case DEPTH_STENCIL_TYPE::NO_DEPTH_TEST:
		_psoDesc.DepthStencilState.DepthEnable = FALSE;
		_psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		break;
	case DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE:
		_psoDesc.DepthStencilState.DepthEnable = FALSE;
		_psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		break;
	}
}

void Shader::ApplyPSOBlendType(BLEND_TYPE type)
{
	D3D12_RENDER_TARGET_BLEND_DESC& rtBlendDesc = _psoDesc.BlendState.RenderTarget[0];

	switch (type)
	{
	case BLEND_TYPE::DEFAULT:
		rtBlendDesc.BlendEnable = FALSE;
		rtBlendDesc.LogicOpEnable = FALSE;
		break;
	case BLEND_TYPE::ALPHA_BLEND:
		rtBlendDesc.BlendEnable = TRUE;
		rtBlendDesc.LogicOpEnable = FALSE;
		rtBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		rtBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		rtBlendDesc.DestBlend = D3D12_BLEND_DEST_ALPHA;
		break;
	case BLEND_TYPE::ONE_TO_ONE_BLEND:
		rtBlendDesc.BlendEnable = TRUE;
		rtBlendDesc.LogicOpEnable = FALSE;
		rtBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		rtBlendDesc.SrcBlend = D3D12_BLEND_ONE;
		rtBlendDesc.DestBlend = D3D12_BLEND_ONE;
		break;
	}
}

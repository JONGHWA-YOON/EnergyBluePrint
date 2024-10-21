#include "GraphicsPch.h"
#include "Shader.h"

//#include "FFGraphicsEngine.h"
#include "Device.h"
#include "RootSignature.h"
#include "CommandQueue.h"

Shader::Shader()
	: IResource(RESOURCE_TYPE::SHADER)
{

}

Shader::~Shader()
{

}

void Shader::CreateGraphicsShader(Device* p_device, RootSignature* p_rootSignature, GraphicsCommandQueue* p_graphicsCommandQueue, ComputeCommandQueue* p_computeCommandQueue, const wstring& p_path, ShaderInfo p_info)
{
	m_device = p_device;
	m_rootSignature = p_rootSignature;
	m_graphicsCommandQueue = p_graphicsCommandQueue;
	m_computeCommandQueue = p_computeCommandQueue;

	m_shaderInfo = p_info;

	if (!p_info.vs.empty())
	{
		CreateVertexShader(p_path, p_info.vs, "vs_5_0");
	}

	if (!p_info.ps.empty())
	{
		CreatePixelShader(p_path, p_info.ps, "ps_5_0");
	}

	if (!p_info.gs.empty())
	{
		CreateGeometryShader(p_path, p_info.gs, "gs_5_0");
	}

	if (!p_info.cs.empty())
	{
		CreateComputeShader(m_device, m_rootSignature, m_graphicsCommandQueue, m_computeCommandQueue, p_path, p_info.cs, "cs_5_0");
		return;
	}

	D3D12_INPUT_ELEMENT_DESC desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		//{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "INDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 60, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

		{"W",0,DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,1},
		{"W",1,DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,1},
		{"W",2,DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,1},
		{"W",3,DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,1},

		{"WV",0,DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,1},
		{"WV",1,DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 80, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,1},
		{"WV",2,DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 96, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,1},
		{"WV",3,DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 112, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,1},

		{"WVP",0,DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 128, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,1},
		{"WVP",1,DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 144, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,1},
		{"WVP",2,DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 160, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,1},
		{"WVP",3,DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 176, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,1},

		{"P",0,DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 192, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,1},
		{"P",1,DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 208, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,1},
		{"P",2,DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 224, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,1},
		{"P",3,DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 240, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,1},
	};

	m_pipelineDesc.InputLayout = { desc, _countof(desc) };
	m_pipelineDesc.pRootSignature = m_rootSignature->GetGraphicsRootSignature().Get();
	m_pipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	m_pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	m_pipelineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	m_pipelineDesc.SampleMask = UINT_MAX;
	m_pipelineDesc.PrimitiveTopologyType = GetTopology(static_cast<D3D_PRIMITIVE_TOPOLOGY>(p_info.topology));
	m_pipelineDesc.NumRenderTargets = 1;
	m_pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_pipelineDesc.SampleDesc.Count = 1;
	m_pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	SetPipelineState();

	m_device->GetDevice()->CreateGraphicsPipelineState(&m_pipelineDesc, IID_PPV_ARGS(&m_pipelineState));

}

void Shader::CreateComputeShader(Device* p_device, RootSignature* p_rootSignature, GraphicsCommandQueue* p_graphicsCommandQueue, ComputeCommandQueue* p_computeCommandQueue, const wstring& p_path, const string& p_name, const string& p_version)
{
	m_device = p_device;
	m_rootSignature = p_rootSignature;
	m_graphicsCommandQueue = p_graphicsCommandQueue;
	m_computeCommandQueue = p_computeCommandQueue;

	m_shaderInfo.shaderType = SHADER_TYPE::COMPUTE;

	CreateShader(p_path, p_name, p_version, m_csBlob, m_computePipelineDesc.CS);
	m_computePipelineDesc.pRootSignature = m_rootSignature->GetComputeRootSignature().Get();

	HRESULT hr = m_device->GetDevice()->CreateComputePipelineState(&m_computePipelineDesc, IID_PPV_ARGS(&m_pipelineState));
	assert(SUCCEEDED(hr));
}

void Shader::Update()
{
	if (m_shaderInfo.shaderType == SHADER_TYPE::COMPUTE)
	{
		m_computeCommandQueue->GetComputeCmdList()->SetPipelineState(m_pipelineState.Get());
	}
	//else if (m_shaderInfo.shaderType == SHADER_TYPE::SHADOW)
	//{
	//	m_graphicsCommandQueue->GetGraphicsCmdList()->IASetPrimitiveTopology(static_cast<D3D_PRIMITIVE_TOPOLOGY>(m_shaderInfo.topology));
	//	m_graphicsCommandQueue->GetGraphicsCmdList()->SetPipelineState(m_shadowPipelineState.Get());
	//}
	else
	{
		m_graphicsCommandQueue->GetGraphicsCmdList()->IASetPrimitiveTopology(static_cast<D3D_PRIMITIVE_TOPOLOGY>(m_shaderInfo.topology));
		m_graphicsCommandQueue->GetGraphicsCmdList()->SetPipelineState(m_pipelineState.Get());
	}
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE Shader::GetTopology(D3D_PRIMITIVE_TOPOLOGY p_topology)
{
	switch (p_topology)
	{
		case D3D_PRIMITIVE_TOPOLOGY_POINTLIST:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		case D3D_PRIMITIVE_TOPOLOGY_LINELIST:
		case D3D_PRIMITIVE_TOPOLOGY_LINESTRIP:
		case D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
		case D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
		case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
		case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
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

void Shader::CreateShader(const wstring& p_path, const string& p_name, const string& p_version, ComPtr<ID3DBlob>& p_blob, D3D12_SHADER_BYTECODE& p_shaderByteCode)
{
	uint32 compileFlag = 0;
#ifdef _DEBUG
	compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	if (FAILED(::D3DCompileFromFile(p_path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, p_name.c_str(), p_version.c_str(), compileFlag, 0, &p_blob, &m_errBlob)))
	{
		::MessageBoxA(nullptr, "Shader Create Failed !", nullptr, MB_OK);
	}

	p_shaderByteCode = { p_blob->GetBufferPointer(), p_blob->GetBufferSize() };
}

void Shader::CreateVertexShader(const wstring& p_path, const string& p_name, const string& p_version)
{
	CreateShader(p_path, p_name, p_version, m_vsBlob, m_pipelineDesc.VS);
}

void Shader::CreatePixelShader(const wstring& p_path, const string& p_name, const string& p_version)
{
	CreateShader(p_path, p_name, p_version, m_psBlob, m_pipelineDesc.PS);
}


void Shader::CreateGeometryShader(const wstring& p_path, const string& p_name, const string& p_version)
{
	CreateShader(p_path, p_name, p_version, m_gsBlob, m_pipelineDesc.GS);
}

void Shader::SetPipelineState()
{
	switch (m_shaderInfo.shaderType)
	{
		case SHADER_TYPE::DEFERRED:
		case SHADER_TYPE::DEFERRED_AND_SKIN:
		{
			m_pipelineDesc.NumRenderTargets = G_BUFFER_MEMBER_COUNT;
			m_pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;	// POSITION
			m_pipelineDesc.RTVFormats[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;	// NORMAL
			m_pipelineDesc.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM;	// COLOR
			m_pipelineDesc.RTVFormats[3] = DXGI_FORMAT_R8G8B8A8_UNORM;	// Depth
		}
		break;
		case SHADER_TYPE::FORWARD:
		{
			m_pipelineDesc.NumRenderTargets = 1;
			m_pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		}
		break;
		case SHADER_TYPE::LIGHTING:
		{
			m_pipelineDesc.NumRenderTargets = 2;
			m_pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;	// Diffuse Light
			m_pipelineDesc.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;	// Specular Light
		}
		break;
		case SHADER_TYPE::PARTICLE:
		{
			m_pipelineDesc.NumRenderTargets = 1;
			m_pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		}
		break;
		case SHADER_TYPE::SHADOW:
		{
			m_pipelineDesc.NumRenderTargets = 1;
			m_pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
			m_pipelineDesc.RasterizerState.DepthBias = 1000;
			m_pipelineDesc.RasterizerState.DepthBiasClamp = 0.0f;
			m_pipelineDesc.RasterizerState.SlopeScaledDepthBias = 1.0f;
		}
		break;
	}

	switch (m_shaderInfo.rasterizerType)
	{
		case RASTERIZER_TYPE::CULL_BACK:
		{
			m_pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
			m_pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
		}
		break;
		case RASTERIZER_TYPE::CULL_FRONT:
		{
			m_pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
			m_pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
		}
		break;
		case RASTERIZER_TYPE::CULL_NONE:
		{
			m_pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
			m_pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		}
		break;
		case RASTERIZER_TYPE::WIREFRAME:
		{
			m_pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
			m_pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		}
		break;
	}

	switch (m_shaderInfo.depthStencilType)
	{
		case DEPTH_STENCIL_TYPE::LESS:
		{
			m_pipelineDesc.DepthStencilState.DepthEnable = TRUE;
			m_pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		}
		break;
		case DEPTH_STENCIL_TYPE::LESS_EQUAL:
		{
			m_pipelineDesc.DepthStencilState.DepthEnable = TRUE;
			m_pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		}
		break;
		case DEPTH_STENCIL_TYPE::GREATER:
		{
			m_pipelineDesc.DepthStencilState.DepthEnable = TRUE;
			m_pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;
		}
		break;
		case DEPTH_STENCIL_TYPE::GREATER_EQUAL:
		{
			m_pipelineDesc.DepthStencilState.DepthEnable = TRUE;
			m_pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		}
		break;
		case DEPTH_STENCIL_TYPE::NO_DEPTH_TEST:
		{
			m_pipelineDesc.DepthStencilState.DepthEnable = FALSE;
			m_pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		}
		break;
		case DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE:
		{
			m_pipelineDesc.DepthStencilState.DepthEnable = FALSE;
			m_pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		}
		break;
		case DEPTH_STENCIL_TYPE::LESS_NO_WRITE:
		{
			m_pipelineDesc.DepthStencilState.DepthEnable = TRUE;
			m_pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
			m_pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		}
		break;
	}

	D3D12_RENDER_TARGET_BLEND_DESC& rt = m_pipelineDesc.BlendState.RenderTarget[0];


	// SrcBlend = Pixel Shader
	// DestBlend = RenderTarget
	switch (m_shaderInfo.blendType)
	{
		case BLEND_TYPE::DEFAULT:
		{
			rt.BlendEnable = FALSE;
			rt.LogicOpEnable = FALSE;
			rt.SrcBlend = D3D12_BLEND_ONE;
			rt.DestBlend = D3D12_BLEND_ZERO;
		}
		break;
		case BLEND_TYPE::ALPHA_BLEND:
		{
			rt.BlendEnable = TRUE;
			rt.LogicOpEnable = FALSE;
			rt.SrcBlend = D3D12_BLEND_SRC_ALPHA;
			rt.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		}
		break;
		case BLEND_TYPE::ONE_TO_ONE_BLEND:
		{
			rt.BlendEnable = TRUE;
			rt.LogicOpEnable = FALSE;
			rt.SrcBlend = D3D12_BLEND_ONE;
			rt.DestBlend = D3D12_BLEND_ONE;
		}
		break;
	}

	//m_pipelineDesc.RasterizerState.DepthBias = 100000;
	//m_pipelineDesc.RasterizerState.DepthBiasClamp = 0.0f;
	//m_pipelineDesc.RasterizerState.SlopeScaledDepthBias = 1.0f;
}

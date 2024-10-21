#include "GraphicsPch.h"
#include "RootSignature.h"

//#include "FFGraphicsEngine.h"
#include "Device.h"
#include "CommandQueue.h"

void RootSignature::Init(Device* p_device, ComputeCommandQueue* p_computeCommandQueue)
{
	m_device = p_device;
	m_computeCommandQueue = p_computeCommandQueue;

	CreateGraphicsRootSignature();
	CreateComputeRootSignature();
}

void RootSignature::CreateGraphicsRootSignature()
{
	m_samlplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	m_samlplerDescArr[0] = CD3DX12_STATIC_SAMPLER_DESC(0);
	m_samlplerDescArr[1] = CD3DX12_STATIC_SAMPLER_DESC(1,
		D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,
		D3D12_TEXTURE_ADDRESS_MODE_BORDER, 
		D3D12_TEXTURE_ADDRESS_MODE_BORDER, 
		0.0f,
		16,
		D3D12_COMPARISON_FUNC_LESS_EQUAL,
		D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK);

	m_samlplerDescArr[2] = CD3DX12_STATIC_SAMPLER_DESC(2,
		D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		0.0f,
		16,
		D3D12_COMPARISON_FUNC_LESS_EQUAL,
		D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK);

	CD3DX12_DESCRIPTOR_RANGE ranges[] =
	{
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, CBV_REGISTER_COUNT-1,1),
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, SRV_REGISTER_COUNT,0),
	};

	CD3DX12_ROOT_PARAMETER param[2];		// 슬롯 -> 레지스터 -> 용도
	//param[0].InitAsConstantBufferView(0);	// 0번 -> b0 -> CBV
	//param[1].InitAsConstantBufferView(1);	// 1번 -> b1 -> CBV
	param[0].InitAsConstantBufferView(static_cast<int32>(CBV_REGISTER::b0));
	param[1].InitAsDescriptorTable(_countof(ranges), ranges);

	D3D12_ROOT_SIGNATURE_DESC _sigDesc = CD3DX12_ROOT_SIGNATURE_DESC(_countof(param), param, 3, m_samlplerDescArr);
	_sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ComPtr<ID3DBlob> _blobSignature;
	ComPtr<ID3DBlob> _blobError;
	::D3D12SerializeRootSignature(&_sigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &_blobSignature, &_blobError);
	m_device->GetDevice()->CreateRootSignature(0, _blobSignature->GetBufferPointer(), _blobSignature->GetBufferSize(),
		IID_PPV_ARGS(&m_graphicsRootSignature));
}

void RootSignature::CreateComputeRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE ranges[] =
	{
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, CBV_REGISTER_COUNT,0),
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, SRV_REGISTER_COUNT,0),
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, UAV_REGISTER_COUNT,0),
	};
	CD3DX12_ROOT_PARAMETER param[1];
	param[0].InitAsDescriptorTable(_countof(ranges), ranges);

	D3D12_ROOT_SIGNATURE_DESC sigDesc = CD3DX12_ROOT_SIGNATURE_DESC(_countof(param), param);
	sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

	ComPtr<ID3DBlob> blobSignature;
	ComPtr<ID3DBlob> blobError;
	::D3D12SerializeRootSignature(&sigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blobSignature, &blobError);
	m_device->GetDevice()->CreateRootSignature(0, blobSignature->GetBufferPointer(), blobSignature->GetBufferSize(), IID_PPV_ARGS(&m_computeRootSignature));

	m_computeCommandQueue->GetComputeCmdList()->SetComputeRootSignature(m_computeRootSignature.Get());
}

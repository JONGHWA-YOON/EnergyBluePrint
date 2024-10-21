#include "GraphicsPch.h"
#include "TableDescriptorHeap.h"

//#include "FFGraphicsEngine.h"
#include "Device.h"
#include "CommandQueue.h"

GraphicsDescriptorHeap::~GraphicsDescriptorHeap()
{
}

void GraphicsDescriptorHeap::Init(uint32 p_count, Device* p_device, GraphicsCommandQueue* p_graphicsCommandQueue)
{
	m_device = p_device;
	m_graphicsCommandQueue = p_graphicsCommandQueue;
	m_groupCount = p_count;

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = p_count * (CBV_SRV_REGISTER_COUNT -1);
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	m_device->GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_descHeap));

	m_handleSize = m_device->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	m_groupSize = m_handleSize * (CBV_SRV_REGISTER_COUNT -1);
}

void GraphicsDescriptorHeap::Clear()
{
	m_currentGroupIndex = 0;
}

void GraphicsDescriptorHeap::SetCBV(D3D12_CPU_DESCRIPTOR_HANDLE p_srcHandle, CBV_REGISTER p_reg)
{
	D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetCPUHandle(p_reg);

	uint32 destRange = 1;
	uint32 srcRange = 1;

	m_device->GetDevice()->CopyDescriptors(1, &destHandle, &destRange,
		1, &p_srcHandle, &srcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void GraphicsDescriptorHeap::SetSRV(D3D12_CPU_DESCRIPTOR_HANDLE p_srcHandle, SRV_REGISTER p_reg)
{
	D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetCPUHandle(p_reg);

	uint32 destRange = 1;
	uint32 srcRange = 1;

	m_device->GetDevice()->CopyDescriptors(1, &destHandle, &destRange,
		1, &p_srcHandle, &srcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void GraphicsDescriptorHeap::CommitTable(bool p_isFinal)
{
	if (p_isFinal == false)
	{
		D3D12_GPU_DESCRIPTOR_HANDLE handle = m_descHeap->GetGPUDescriptorHandleForHeapStart();
		handle.ptr += m_currentGroupIndex * m_groupSize;
		m_graphicsCommandQueue->GetGraphicsCmdList()->SetGraphicsRootDescriptorTable(1, handle);
	}
	else
	{
		m_finalGPUHandle = m_descHeap->GetGPUDescriptorHandleForHeapStart();
		m_finalGPUHandle.ptr += m_currentGroupIndex * m_groupSize;
		m_graphicsCommandQueue->GetGraphicsCmdList()->SetGraphicsRootDescriptorTable(1, m_finalGPUHandle);
	}

	m_currentGroupIndex++;
}

D3D12_CPU_DESCRIPTOR_HANDLE GraphicsDescriptorHeap::GetCPUHandle(CBV_REGISTER p_reg)
{
	return GetCPUHandle(static_cast<uint8>(p_reg));
}

D3D12_CPU_DESCRIPTOR_HANDLE GraphicsDescriptorHeap::GetCPUHandle(SRV_REGISTER p_reg)
{
	return GetCPUHandle(static_cast<uint8>(p_reg));
}

D3D12_CPU_DESCRIPTOR_HANDLE GraphicsDescriptorHeap::GetCPUHandle(uint8 p_reg)
{
	assert(p_reg > 0);
	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_descHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += m_currentGroupIndex * m_groupSize;
	handle.ptr +=(p_reg-1) * m_handleSize;
	return handle;
}

/// Compute¿ë DescriptorHeap

void ComputeDescriptorHeap::Init(Device* p_device, ComputeCommandQueue* p_computeCommandQueue)
{
	m_device = p_device;
	m_computeCommandQueue = p_computeCommandQueue;

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = TOTAL_REGISTER_COUNT;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	m_device->GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_descHeap));

	m_handleSize =m_device->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void ComputeDescriptorHeap::SetCBV(D3D12_CPU_DESCRIPTOR_HANDLE p_srcHandle, CBV_REGISTER p_reg)
{
	D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetCPUHandle(p_reg);

	uint32 destRange = 1;
	uint32 srcRange = 1;
	m_device->GetDevice()->CopyDescriptors(1, &destHandle, &destRange, 1, &p_srcHandle, &srcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void ComputeDescriptorHeap::SetSRV(D3D12_CPU_DESCRIPTOR_HANDLE p_srcHandle, SRV_REGISTER p_reg)
{
	D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetCPUHandle(p_reg);

	uint32 destRange = 1;
	uint32 srcRange = 1;
	m_device->GetDevice()->CopyDescriptors(1, &destHandle, &destRange, 1, &p_srcHandle, &srcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void ComputeDescriptorHeap::SetUAV(D3D12_CPU_DESCRIPTOR_HANDLE p_srcHandle, UAV_REGISTER p_reg)
{
	D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetCPUHandle(p_reg);

	uint32 destRange = 1;
	uint32 srcRange = 1;
	m_device->GetDevice()->CopyDescriptors(1, &destHandle, &destRange, 1, &p_srcHandle, &srcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void ComputeDescriptorHeap::CommitTable()
{
	ID3D12DescriptorHeap* descHeap = m_descHeap.Get();
	m_computeCommandQueue->GetComputeCmdList()->SetDescriptorHeaps(1, &descHeap);

	D3D12_GPU_DESCRIPTOR_HANDLE handle = descHeap->GetGPUDescriptorHandleForHeapStart();
	m_computeCommandQueue->GetComputeCmdList()->SetComputeRootDescriptorTable(0, handle);
}

D3D12_CPU_DESCRIPTOR_HANDLE ComputeDescriptorHeap::GetCPUHandle(CBV_REGISTER p_reg)
{
	return GetCPUHandle(static_cast<uint8>(p_reg));
}

D3D12_CPU_DESCRIPTOR_HANDLE ComputeDescriptorHeap::GetCPUHandle(SRV_REGISTER p_reg)
{
	return GetCPUHandle(static_cast<uint8>(p_reg));
}

D3D12_CPU_DESCRIPTOR_HANDLE ComputeDescriptorHeap::GetCPUHandle(UAV_REGISTER p_reg)
{
	return GetCPUHandle(static_cast<uint8>(p_reg));
}

D3D12_CPU_DESCRIPTOR_HANDLE ComputeDescriptorHeap::GetCPUHandle(uint8 p_reg)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_descHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += p_reg * m_handleSize;
	return handle;
}

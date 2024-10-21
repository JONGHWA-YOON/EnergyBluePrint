#include "GraphicsPch.h"
#include "StructuredBuffer.h"

//#include "FFGraphicsEngine.h"
#include "Device.h"
#include "TableDescriptorHeap.h"

#include "ResourceManager.h"
#include "CommandQueue.h"

StructuredBuffer::StructuredBuffer()
{

}

StructuredBuffer::~StructuredBuffer()
{

}

void StructuredBuffer::Init(uint32 p_elementSize, uint32 p_elementCount, Device* p_device, GraphicsDescriptorHeap* p_graphicsDescriptorHeap, ComputeDescriptorHeap* p_computeDescriptorHeap, void* initialData)
{
	m_device = p_device;
	m_graphicsDescriptorHeap = p_graphicsDescriptorHeap;
	m_computeDescriptorHeap = p_computeDescriptorHeap;

	m_elementSize = p_elementSize;
	m_elementCount = p_elementCount;
	m_resourceState = D3D12_RESOURCE_STATE_COMMON;

	// Buffer
	{
		uint64 bufferSize = static_cast<uint64>(m_elementSize) * m_elementCount;
		D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
		D3D12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		m_device->GetDevice()->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			m_resourceState,
			nullptr,
			IID_PPV_ARGS(&m_buffer));

		if (initialData)
		{
			CopyInitialData(bufferSize, initialData);
		}
	}

	// SRV
	{
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = 1;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		m_device->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_srvHeap));

		m_srvHeapBegin = m_srvHeap->GetCPUDescriptorHandleForHeapStart();

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = m_elementCount;
		srvDesc.Buffer.StructureByteStride = m_elementSize;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

		m_device->GetDevice()->CreateShaderResourceView(m_buffer.Get(), &srvDesc, m_srvHeapBegin);
	}

	// UAV
	{
		D3D12_DESCRIPTOR_HEAP_DESC uavHeapDesc = {};
		uavHeapDesc.NumDescriptors = 1;
		uavHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		uavHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		m_device->GetDevice()->CreateDescriptorHeap(&uavHeapDesc, IID_PPV_ARGS(&m_uavHeap));

		m_uavHeapBegin = m_uavHeap->GetCPUDescriptorHandleForHeapStart();

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = m_elementCount;
		uavDesc.Buffer.StructureByteStride = m_elementSize;
		uavDesc.Buffer.CounterOffsetInBytes = 0;
		uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

		m_device->GetDevice()->CreateUnorderedAccessView(m_buffer.Get(), nullptr, &uavDesc, m_uavHeapBegin);
	}
}

void StructuredBuffer::PushGraphicsData(SRV_REGISTER p_reg)
{
	m_graphicsDescriptorHeap->SetSRV(m_srvHeapBegin, p_reg);
}

void StructuredBuffer::PushComputeSRVData(SRV_REGISTER p_reg)
{
	m_computeDescriptorHeap->SetSRV(m_srvHeapBegin, p_reg);
}

void StructuredBuffer::PushComputeUAVData(UAV_REGISTER p_reg)
{
	m_computeDescriptorHeap->SetUAV(m_uavHeapBegin, p_reg);
}

void StructuredBuffer::CopyInitialData(uint64 bufferSize, void* initialData)
{
	ComPtr<ID3D12Resource> readBuffer = nullptr;
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, D3D12_RESOURCE_FLAG_NONE);
	D3D12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	m_device->GetDevice()->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&readBuffer));

	uint8* dataBegin = nullptr;
	D3D12_RANGE readRange{ 0, 0 };
	readBuffer->Map(0, &readRange, reinterpret_cast<void**>(&dataBegin));
	memcpy(dataBegin, initialData, bufferSize);
	readBuffer->Unmap(0, nullptr);

	// Common -> Copy
	{
		D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_buffer.Get(),
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);

		ResourceManager::GetInstance()->GetGraphicsCmdQueue()->GetResourceCmdList()->ResourceBarrier(1, &barrier);
	}

	ResourceManager::GetInstance()->GetGraphicsCmdQueue()->GetResourceCmdList()->CopyBufferRegion(m_buffer.Get(), 0, readBuffer.Get(), 0, bufferSize);

	// Copy -> Common
	{
		D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_buffer.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON);
		ResourceManager::GetInstance()->GetGraphicsCmdQueue()->GetResourceCmdList()->ResourceBarrier(1, &barrier);
	}

	ResourceManager::GetInstance()->GetGraphicsCmdQueue()->FlushResourceCommandQueue();

	m_resourceState = D3D12_RESOURCE_STATE_COMMON;
}

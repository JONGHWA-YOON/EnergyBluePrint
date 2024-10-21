#include "GraphicsPch.h"
#include "ConstantBuffer.h"

//#include "FFGraphicsEngine.h"
#include "TableDescriptorHeap.h"
#include "Device.h"
#include "CommandQueue.h"

ConstantBuffer::ConstantBuffer()
{

}

ConstantBuffer::~ConstantBuffer()
{
	if (m_cbvBuffer)
	{
		if (m_cbvBuffer != nullptr)
		{
			m_cbvBuffer->Unmap(0, nullptr);
		}

		m_cbvBuffer = nullptr;
	}
}

void ConstantBuffer::Init(CBV_REGISTER p_reg, uint32 p_size, uint32 p_count,Device* p_device, GraphicsDescriptorHeap* p_graphicsDescriptorHeap, ComputeDescriptorHeap* p_computeDescriptorHeap, GraphicsCommandQueue* p_graphicsCommandQueue)
{
	m_device = p_device;
	m_graphicsDescriptorHeap = p_graphicsDescriptorHeap;
	m_computeDescriptorHeap = p_computeDescriptorHeap;
	m_graphicsCommandQueue = p_graphicsCommandQueue;

	m_reg = p_reg;

	// 상수 버퍼는 256바이트 배수로 만들어야 한다
	// 0 256	512	768
	m_elementSize = (p_size + 255) & ~255;
	m_elementCount = p_count;

	CreateBuffer();
	CreateView();
}

void ConstantBuffer::Clear()
{
	m_currentIndex = 0;
}

void ConstantBuffer::PushGraphicsData(void* p_buffer, uint32 p_size)
{
	assert(m_currentIndex < m_elementCount);
	assert(m_elementSize == ((p_size + 255) & ~255));

	::memcpy(&m_mappedBuffer[m_currentIndex * m_elementSize], p_buffer, p_size);

	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = GetCpuHandle(m_currentIndex);

	m_graphicsDescriptorHeap->SetCBV(cpuHandle, m_reg);

	m_currentIndex++;
}

void ConstantBuffer::PushComputeSRVData(void* p_buffer, uint32 p_size)
{
	assert(m_currentIndex < m_elementCount);
	assert(m_elementSize == ((p_size + 255) & ~255));

	::memcpy(&m_mappedBuffer[m_currentIndex * m_elementSize], p_buffer, p_size);

	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = GetCpuHandle(m_currentIndex);
	m_computeDescriptorHeap->SetCBV(cpuHandle, m_reg);

	m_currentIndex++;
}

void ConstantBuffer::SetGlobalData(void* p_buffer, uint32 p_size)
{
	assert(m_elementSize == ((p_size + 255) & ~255));
	::memcpy(&m_mappedBuffer[0], p_buffer, p_size);
	m_graphicsCommandQueue->GetGraphicsCmdList()->SetGraphicsRootConstantBufferView(0, GetGPUVirtualAddress(0));
}

D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer::GetGPUVirtualAddress(uint32 p_index)
{
	D3D12_GPU_VIRTUAL_ADDRESS _objCBAddress = m_cbvBuffer->GetGPUVirtualAddress();
	_objCBAddress += p_index * m_elementSize;
	return _objCBAddress;
}

D3D12_CPU_DESCRIPTOR_HANDLE ConstantBuffer::GetCpuHandle(uint32 p_index)
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_cpuHandleBegin, p_index * m_handleIncrementSize);
}

void ConstantBuffer::CreateBuffer()
{
	uint32 _bufferSize = m_elementSize * m_elementCount;
	D3D12_HEAP_PROPERTIES _heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC _desc = CD3DX12_RESOURCE_DESC::Buffer(_bufferSize);

	m_device->GetDevice()->CreateCommittedResource(
		&_heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&_desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_cbvBuffer));

	m_cbvBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedBuffer));
}

void ConstantBuffer::CreateView()
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvDesc = {};
	cbvDesc.NumDescriptors = m_elementCount;
	cbvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	cbvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	m_device->GetDevice()->CreateDescriptorHeap(&cbvDesc, IID_PPV_ARGS(&m_cbvHeap));

	m_cpuHandleBegin = m_cbvHeap->GetCPUDescriptorHandleForHeapStart();
	m_handleIncrementSize = m_device->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	for (uint32 i = 0; i < m_elementCount; ++i)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE cbvHandle = GetCpuHandle(i);

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = m_cbvBuffer->GetGPUVirtualAddress() + static_cast<uint64>(m_elementSize) * i;
		cbvDesc.SizeInBytes = m_elementSize;

		m_device->GetDevice()->CreateConstantBufferView(&cbvDesc, cbvHandle);
		// 여기서 ID3D12Resource형의 m_cbvBuffer와 ID3D12DescriptorHeap형의 m_cbvHeap이 연결 될 수 있는 이유는
		// cbvDesc.BufferLocation = m_cbvBuffer->GetGPUVirtualAddress() + static_cast<uint64>(m_elementSize) * i;
		// 때문이다. 
	}
}

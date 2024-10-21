#pragma once

class Device;
class GraphicsDescriptorHeap;
class ComputeDescriptorHeap;

class StructuredBuffer
{
public:
	StructuredBuffer();
	~StructuredBuffer();

	void Init(uint32 p_elementSize, uint32 p_elementCount, Device* p_device, GraphicsDescriptorHeap* p_graphicsDescriptorHeap, ComputeDescriptorHeap* p_computeDescriptorHeap, void* initialData = nullptr);

	void PushGraphicsData(SRV_REGISTER p_reg);
	void PushComputeSRVData(SRV_REGISTER p_reg);
	void PushComputeUAVData(UAV_REGISTER p_reg);

	ComPtr<ID3D12DescriptorHeap> GetSRV() { return m_srvHeap; }
	ComPtr<ID3D12DescriptorHeap> GetUAV() { return m_uavHeap; }

	void SetResourceState(D3D12_RESOURCE_STATES p_state) { m_resourceState = p_state; }
	D3D12_RESOURCE_STATES GetResourceState() { return m_resourceState; }
	ComPtr<ID3D12Resource> GetBuffer() { return m_buffer; }

	uint32	GetElementSize() { return m_elementSize; }
	uint32	GetElementCount() { return m_elementCount; }
	UINT	GetBufferSize() { return m_elementSize * m_elementCount; }

private:
	void CopyInitialData(uint64 bufferSize, void* initialData);

private:
	ComPtr<ID3D12Resource> m_buffer;
	ComPtr<ID3D12DescriptorHeap> m_srvHeap;
	ComPtr<ID3D12DescriptorHeap> m_uavHeap;

	uint32 m_elementSize = 0;
	uint32 m_elementCount = 0;
	D3D12_RESOURCE_STATES m_resourceState = {};

private:
	D3D12_CPU_DESCRIPTOR_HANDLE m_srvHeapBegin = {};
	D3D12_CPU_DESCRIPTOR_HANDLE m_uavHeapBegin = {};

private:
	// 참조용 변수들
	Device* m_device;
	GraphicsDescriptorHeap* m_graphicsDescriptorHeap;
	ComputeDescriptorHeap* m_computeDescriptorHeap;
};


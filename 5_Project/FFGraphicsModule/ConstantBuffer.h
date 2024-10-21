#pragma once

class Device;
class GraphicsDescriptorHeap;
class ComputeDescriptorHeap;
class GraphicsCommandQueue;

/// <summary>
/// 상수 버퍼 클래스 
/// 여기서는 실질적인 데이터가 들어간 Buffer와 그 Buffer를 가리키는 DrecriptorHeap을 정의한다.
/// 
/// 실질적인 데이터는 ID3D12Resource의 Map함수로 연결하고 ID3D12Resource와 View는
/// Device의 CreateConstantBufferView함수로 연결한다.
/// 
/// 2023.06.16 김성찬 게임인재원
/// </summary>
class ConstantBuffer
{
public:
	ConstantBuffer();
	// Map으로 연결된 것을 Unmap한다.
	~ConstantBuffer();

	// Buffer와 View를 만든다.
	void Init(CBV_REGISTER p_reg, uint32 p_size, uint32 p_count, Device* p_device, GraphicsDescriptorHeap* p_graphicsDescriptorHeap, ComputeDescriptorHeap* p_computeDescriptorHeap, GraphicsCommandQueue* p_graphicsCommandQueue);

	// 매 프레임 호출되며 인덱스를 초기화 한다.
	void Clear();

	// 실질적인 데이터를 넣는다. memcpy이용
	void PushGraphicsData(void* p_buffer, uint32 p_size);
	void PushComputeSRVData(void* p_buffer, uint32 p_size);

	void SetGlobalData(void* p_buffer, uint32 p_size);

#pragma region Getter
	// 원하는 위치의 GPU주소를 가져온다.
	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress(uint32 p_index);

	// 원하는 위치의 CPU Handle을 가져온다.
	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(uint32 p_index);
#pragma endregion

private:
	void CreateBuffer();
	void CreateView();

private:
	ComPtr<ID3D12Resource> m_cbvBuffer;
	BYTE* m_mappedBuffer = nullptr;
	uint32 m_elementSize = 0;
	uint32 m_elementCount = 0;

	ComPtr<ID3D12DescriptorHeap> m_cbvHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE m_cpuHandleBegin = {};
	uint32 m_handleIncrementSize = 0;

	uint32 m_currentIndex = 0;

	CBV_REGISTER m_reg = {};

private:
	// 참조용 변수들
	Device* m_device;
	GraphicsDescriptorHeap* m_graphicsDescriptorHeap;
	ComputeDescriptorHeap* m_computeDescriptorHeap;
	GraphicsCommandQueue* m_graphicsCommandQueue;
};


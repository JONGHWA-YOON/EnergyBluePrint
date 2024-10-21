#pragma once

class Device;
class GraphicsCommandQueue;
class ComputeCommandQueue;

/// <summary>
/// 모든 Descriptor들을 관리하는 Table
/// 
/// 요놈을 나중에 미리 만들어둔 Rootsignature에 복사하여 레지스터에 할당한다.
/// 
/// 2023.06.16 김성찬 게임인재원
/// </summary>
class GraphicsDescriptorHeap
{
public:
	~GraphicsDescriptorHeap();

public:
	// 전체적인 Descriptor들을 관리할 DescriptorHeap을 만든다. 
	void Init(uint32 p_count, Device* p_device, GraphicsCommandQueue* p_graphicsCommandQueue);

	// 매 프레임 불리면서 index를 0으로 바꿔준다.
	void Clear();

	// ConstantBufferView를 세팅한다. 
	// 내부적으로 현재 인덱스와 레지스터만큼 테이블을 이동하여 적절한 위치에 세팅해준다.
	void SetCBV(D3D12_CPU_DESCRIPTOR_HANDLE p_srcHandle, CBV_REGISTER p_reg);

	// ShaderResourceView를 세팅한다.
	// 내부적으로 현재 인덱스와 레지스터만큼 테이블을 이동하여 적절한 위치에 세팅해준다.
	void SetSRV(D3D12_CPU_DESCRIPTOR_HANDLE p_srcHandle, SRV_REGISTER p_reg);

	// 이 함수를 불러서 명령 목록에 현재 인덱스에 해당하는 Handle을 제출한다.
	void CommitTable(bool p_isFinal = false);

	unsigned __int64 GetGPUHandle() { return m_finalGPUHandle.ptr; }

#pragma region Getter
	ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() { return m_descHeap; }

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(CBV_REGISTER p_reg);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(SRV_REGISTER p_reg);
#pragma endregion

private:
	// 클래스 내부에서만 사용되며 현재 인덱스에 해당하는 Handle을 반환한다.
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint8 p_reg);

private:
	ComPtr<ID3D12DescriptorHeap> m_descHeap;
	uint64 m_handleSize = 0;
	uint64 m_groupSize = 0;
	uint64 m_groupCount = 0;

	uint32 m_currentGroupIndex = 0;

private:
	D3D12_GPU_DESCRIPTOR_HANDLE m_finalGPUHandle;

private:
	// 참조용 변수들
	Device* m_device;
	GraphicsCommandQueue* m_graphicsCommandQueue;
};

/// Compute용 DescriptorHeap

class ComputeDescriptorHeap
{
public:
	void Init(Device* p_device, ComputeCommandQueue* p_computeCommandQueue);

	void SetCBV(D3D12_CPU_DESCRIPTOR_HANDLE p_srcHandle, CBV_REGISTER p_reg);
	void SetSRV(D3D12_CPU_DESCRIPTOR_HANDLE p_srcHandle, SRV_REGISTER p_reg);
	void SetUAV(D3D12_CPU_DESCRIPTOR_HANDLE p_srcHandle, UAV_REGISTER p_reg);

	void CommitTable();

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(CBV_REGISTER p_reg);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(SRV_REGISTER p_reg);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(UAV_REGISTER p_reg);

private:
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint8 p_reg);

private:
	ComPtr<ID3D12DescriptorHeap> m_descHeap;
	uint64 m_handleSize = 0;

private:
	// 참조용 변수들
	Device* m_device;
	ComputeCommandQueue* m_computeCommandQueue;
};
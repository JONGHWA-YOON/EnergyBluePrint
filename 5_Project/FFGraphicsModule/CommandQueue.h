#pragma once

class Device;
class ConstantBuffer;
class GraphicsDescriptorHeap;
class SwapChain;
class RenderTargetGroup;
class RootSignature;

/// <summary>
/// GPU에서 실질적으로 실행될 명령 목록을 담는 Queue
/// 마지막에 Excute해서 CommandList에 있는 명령 목록을 GPU에 넘긴다.
/// 그렇기에 CommandQueue Class에 CommandList도 가지고 있는다
/// 
/// 2023.06.16 김성찬 게임인재원
/// </summary>

class GraphicsCommandQueue
{
public:
	~GraphicsCommandQueue();

	// 초기화를 진행한다.
	void Init(Device* p_device, vector<ConstantBuffer*>* p_constantBuffers, GraphicsDescriptorHeap* p_graphicsDescriptorHeap, SwapChain* p_swapChain, array<RenderTargetGroup*, RENDER_TARGET_TYPE_COUNT>* p_renderTargets, RootSignature* p_rootSignature);

	// GPU와 CPU를 동기화 해준다.
	void WaitSync();

	// 그리기 시작
	void RenderBegin(const D3D12_VIEWPORT* p_vp, const D3D12_RECT* p_rect);

	// 그리기 끝
	void RenderEnd();

	// 리소스용 CommandList를 다시 
	void FlushResourceCommandQueue();

#pragma region Getter
	ComPtr<ID3D12CommandQueue> GetGraphicsCmdQueue() { return m_graphicsCmdQueue; }
	ComPtr<ID3D12GraphicsCommandList> GetGraphicsCmdList() { return m_cmdList; }
	ComPtr<ID3D12GraphicsCommandList> GetResourceCmdList() { return m_resCmdList; }
#pragma endregion

	
private:
	// 실질적인 CommandQueue
	ComPtr<ID3D12CommandQueue> m_graphicsCmdQueue;

	// 전체적인 명령 목록 (리소스 로드 제외)을 담당할 List
	ComPtr<ID3D12CommandAllocator> m_cmdAlloc;
	ComPtr<ID3D12GraphicsCommandList> m_cmdList;

	// 리소스를 로드하는 용도로 쓰일 커맨드 리스트
	ComPtr<ID3D12CommandAllocator> m_resCmdAlloc;
	ComPtr<ID3D12GraphicsCommandList> m_resCmdList;

	// CPU / GPU 동기화를 위한 간단한 도구
	ComPtr<ID3D12Fence> m_fence;
	uint32 m_fenceValue = 0;
	HANDLE m_fenceEvent = INVALID_HANDLE_VALUE;

private:
	// 참조용 변수들
	Device* m_device;
	vector<ConstantBuffer*>* m_constantBuffers;
	GraphicsDescriptorHeap* m_graphicsDescriptorHeap;
	SwapChain* m_swapChain;
	array<RenderTargetGroup*, RENDER_TARGET_TYPE_COUNT>* m_renderTargets;
	RootSignature* m_rootSignature;
};

/// Compute용 CommandQueue

class ComputeCommandQueue
{
public:
	~ComputeCommandQueue();

	void Init(Device* p_device, RootSignature* p_rootSignature);
	void WaitSync();
	void FlushComputeCommandQueue();

	ComPtr<ID3D12CommandQueue> GetComputeCmdQueue() { return m_computeCmdQueue; }
	ComPtr<ID3D12GraphicsCommandList> GetComputeCmdList() { return m_cmdList; }

private:
	ComPtr<ID3D12CommandQueue> m_computeCmdQueue;
	ComPtr<ID3D12CommandAllocator> m_cmdAlloc;
	ComPtr<ID3D12GraphicsCommandList> m_cmdList;

	ComPtr<ID3D12Fence> m_fence;
	uint32 m_fenceValue = 0;
	HANDLE m_fenceEvent = INVALID_HANDLE_VALUE;

private:
	// 참조용 변수들
	Device* m_device;
	RootSignature* m_rootSignature;
};
#include "GraphicsPch.h"
#include "CommandQueue.h"

//#include "FFGraphicsEngine.h"
#include "Device.h"
#include "SwapChain.h"
#include "RootSignature.h"
#include "TableDescriptorHeap.h"
#include "ConstantBuffer.h"
#include "RenderTargetGroup.h"
#include "Texture.h"

GraphicsCommandQueue::~GraphicsCommandQueue()
{
	::CloseHandle(m_fenceEvent);
}

void GraphicsCommandQueue::Init(Device* p_device, vector<ConstantBuffer*>* p_constantBuffers, GraphicsDescriptorHeap* p_graphicsDescriptorHeap, SwapChain* p_swapChain, array<RenderTargetGroup*, RENDER_TARGET_TYPE_COUNT>* p_renderTargets, RootSignature* p_rootSignature)
{
	m_device = p_device;
	m_constantBuffers = p_constantBuffers;
	m_graphicsDescriptorHeap = p_graphicsDescriptorHeap;
	m_swapChain = p_swapChain;
	m_renderTargets = p_renderTargets;
	m_rootSignature = p_rootSignature;

	D3D12_COMMAND_QUEUE_DESC _queueDesc = {};
	_queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	_queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	m_device->GetDevice()->CreateCommandQueue(&_queueDesc, IID_PPV_ARGS(&m_graphicsCmdQueue));

	// D3D12_COMMAND_LIST_TYPE_DIRECT : GPU가 직접 실행하는 명령 목록
	m_device->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_cmdAlloc));

	// GPU가 하나인 시스템에서는 0으로
	// DIRECT or BUNDLE
	// Allocator
	// 초기 상태 (그리기 명령은 nullptr 지정)
	m_device->GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_cmdAlloc.Get(), nullptr, IID_PPV_ARGS(&m_cmdList));

	// CommandList는 Close / Open 상태가 있는데
	// Open 상태에서 Command를 넣다가 Close한 다음 제출하는 개념
	m_cmdList->Close();

	// 리소스 로드할 때 쓰이는 커맨드 리스트 생성
	m_device->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_resCmdAlloc));
	m_device->GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_resCmdAlloc.Get(), nullptr, IID_PPV_ARGS(&m_resCmdList));

	// CreateFence
	// CPU와 GPU의 동기화 수단으로 쓰임
	m_device->GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
	m_fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void GraphicsCommandQueue::WaitSync()
{
	m_fenceValue++;

	m_graphicsCmdQueue->Signal(m_fence.Get(), m_fenceValue);

	if (m_fence->GetCompletedValue() < m_fenceValue)
	{
		m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent);

		::WaitForSingleObject(m_fenceEvent, INFINITE);
	}
}

void GraphicsCommandQueue::RenderBegin(const D3D12_VIEWPORT* p_vp, const D3D12_RECT* p_rect)
{
	m_cmdAlloc->Reset();
	m_cmdList->Reset(m_cmdAlloc.Get(), nullptr);

	int8 backIndex = m_swapChain->GetBackBufferIndex();

	D3D12_RESOURCE_BARRIER _barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		(*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::SWAP_CHAIN)]->GetRTTexture(backIndex)->GetTex2D().Get(),
		D3D12_RESOURCE_STATE_PRESENT,	// 화면 출력
		D3D12_RESOURCE_STATE_RENDER_TARGET);	// 외주 결과물

	m_cmdList->SetGraphicsRootSignature(m_rootSignature->GetGraphicsRootSignature().Get());
	(*m_constantBuffers)[static_cast<uint8>(CONSTANT_BUFFER_TYPE::TRANSFORM)]->Clear();
	(*m_constantBuffers)[static_cast<uint8>(CONSTANT_BUFFER_TYPE::MATERIAL)]->Clear();
	m_graphicsDescriptorHeap->Clear();

	ID3D12DescriptorHeap* descHeap = m_graphicsDescriptorHeap->GetDescriptorHeap().Get();
	m_cmdList->SetDescriptorHeaps(1, &descHeap);

	m_cmdList->ResourceBarrier(1, &_barrier);

	//m_cmdList->RSSetViewports(1, p_vp);
	//m_cmdList->RSSetScissorRects(1, p_rect);
}

void GraphicsCommandQueue::RenderEnd()
{
	int8 backIndex = m_swapChain->GetBackBufferIndex();

	D3D12_RESOURCE_BARRIER _barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		(*m_renderTargets)[static_cast<uint8>(RENDER_TARGET_TYPE::SWAP_CHAIN)]->GetRTTexture(backIndex)->GetTex2D().Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,	// 외주 결과물
		D3D12_RESOURCE_STATE_PRESENT);	// 화면 출력

	m_cmdList->ResourceBarrier(1, &_barrier);
	m_cmdList->Close();

	// 커맨드 리스트 수행
	ID3D12CommandList* _cmdListArr[] = { m_cmdList.Get() };
	m_graphicsCmdQueue->ExecuteCommandLists(_countof(_cmdListArr), _cmdListArr);
	                                                    
	m_swapChain->Present();

	WaitSync();

	m_swapChain->SwapIndex();
}

void GraphicsCommandQueue::FlushResourceCommandQueue()
{
	m_resCmdList->Close();

	ID3D12CommandList* cmdListArr[] = { m_resCmdList.Get() };
	m_graphicsCmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

	WaitSync();

	m_resCmdAlloc->Reset();
	m_resCmdList->Reset(m_resCmdAlloc.Get(), nullptr);
}

/// Compute용 CommandQueue

ComputeCommandQueue::~ComputeCommandQueue()
{
	::CloseHandle(m_fenceEvent);
}

void ComputeCommandQueue::Init(Device* p_device, RootSignature* p_rootSignature)
{
	m_device = p_device;
	m_rootSignature = p_rootSignature;

	D3D12_COMMAND_QUEUE_DESC computeQueueDesc = {};
	computeQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
	computeQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	m_device->GetDevice()->CreateCommandQueue(&computeQueueDesc, IID_PPV_ARGS(&m_computeCmdQueue));

	m_device->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&m_cmdAlloc));
	m_device->GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, m_cmdAlloc.Get(), nullptr, IID_PPV_ARGS(&m_cmdList));

	m_device->GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
	m_fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void ComputeCommandQueue::WaitSync()
{
	m_fenceValue++;

	HRESULT hr = m_computeCmdQueue->Signal(m_fence.Get(), m_fenceValue);

	if (hr != S_OK)
	{
		int a = 1;
	}

	if (m_fence->GetCompletedValue() < m_fenceValue)
	{
		m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent);
		::WaitForSingleObject(m_fenceEvent, INFINITE);
	}
}

void ComputeCommandQueue::FlushComputeCommandQueue()
{
	m_cmdList->Close();

	ID3D12CommandList* cmdListArr[] = { m_cmdList.Get() };
	auto t = _countof(cmdListArr);
	m_computeCmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);




	WaitSync();

	m_cmdAlloc->Reset();
	m_cmdList->Reset(m_cmdAlloc.Get(), nullptr);

	m_cmdList->SetComputeRootSignature(m_rootSignature->GetComputeRootSignature().Get());
}

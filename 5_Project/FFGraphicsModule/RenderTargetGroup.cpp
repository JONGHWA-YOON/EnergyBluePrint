#include "GraphicsPch.h"
#include "RenderTargetGroup.h"
//#include "FFGraphicsEngine.h"
#include "Device.h"
#include "Texture.h"
#include "CommandQueue.h"

RenderTargetGroup::~RenderTargetGroup()
{
	//for (auto& i : m_vecRenderTarget)
	//{
	//	delete i.texture;
	//}
}

void RenderTargetGroup::Create(RENDER_TARGET_TYPE p_type, vector<RenderTarget>& p_vecRT, Texture* p_dsTexture, Device* p_device, GraphicsCommandQueue* p_graphicsCommandQueue)
{
	m_device = p_device;
	m_graphicsCommandQueue = p_graphicsCommandQueue;

	m_groupType = p_type;
	m_vecRenderTarget = p_vecRT;
	m_renderTargetCount = static_cast<uint32>(p_vecRT.size());

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.NumDescriptors = m_renderTargetCount;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	heapDesc.NodeMask = 0;

	m_device->GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_rtvHeap));

	m_rtvHeapSize =m_device->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_rtvHeapBegin = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
	m_dsvHeapBegin = p_dsTexture->GetDSV()->GetCPUDescriptorHandleForHeapStart();

	for (uint32 i = 0; i < m_renderTargetCount; ++i)
	{
		uint32 destSize = 1;
		D3D12_CPU_DESCRIPTOR_HANDLE destHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rtvHeapBegin, i * m_rtvHeapSize);

		uint32 srcSize = 1;
		ComPtr<ID3D12DescriptorHeap> srcRtvHeapBegin = m_vecRenderTarget[i].texture->GetRTV();
		D3D12_CPU_DESCRIPTOR_HANDLE srcHandle = srcRtvHeapBegin->GetCPUDescriptorHandleForHeapStart();

		m_device->GetDevice()->CopyDescriptors(1, &destHandle, &destSize, 1, &srcHandle, &srcSize, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	for (int i = 0; i < m_renderTargetCount; ++i)
	{
		m_targetToResource[i] = CD3DX12_RESOURCE_BARRIER::Transition(m_vecRenderTarget[i].texture->GetTex2D().Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON);

		m_resourceToTarget[i] = CD3DX12_RESOURCE_BARRIER::Transition(m_vecRenderTarget[i].texture->GetTex2D().Get(),
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET);
	}
}

void RenderTargetGroup::OMSetRenderTargets(uint32 p_count, uint32 p_offset)
{
	D3D12_VIEWPORT vp = D3D12_VIEWPORT{ 0.f, 0.f, m_vecRenderTarget[0].texture->GetWidth() , m_vecRenderTarget[0].texture->GetHeight(), 0.f, 1.f };
	D3D12_RECT rect = D3D12_RECT{ 0, 0, static_cast<LONG>(m_vecRenderTarget[0].texture->GetWidth()),  static_cast<LONG>(m_vecRenderTarget[0].texture->GetHeight()) };

	m_graphicsCommandQueue->GetGraphicsCmdList()->RSSetViewports(1, &vp);
	m_graphicsCommandQueue->GetGraphicsCmdList()->RSSetScissorRects(1, &rect);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rtvHeapBegin, p_offset * m_rtvHeapSize);
	m_graphicsCommandQueue->GetGraphicsCmdList()->OMSetRenderTargets(p_count, &rtvHandle, FALSE, &m_dsvHeapBegin);
}

void RenderTargetGroup::OMSetRenderTargets()
{
	D3D12_VIEWPORT vp = D3D12_VIEWPORT{ 0.f, 0.f, m_vecRenderTarget[0].texture->GetWidth() , m_vecRenderTarget[0].texture->GetHeight(), 0.f, 1.f };
	D3D12_RECT rect = D3D12_RECT{ 0, 0, static_cast<LONG>(m_vecRenderTarget[0].texture->GetWidth()),  static_cast<LONG>(m_vecRenderTarget[0].texture->GetHeight()) };

	m_graphicsCommandQueue->GetGraphicsCmdList()->RSSetViewports(1, &vp);
	m_graphicsCommandQueue->GetGraphicsCmdList()->RSSetScissorRects(1, &rect);

	m_graphicsCommandQueue->GetGraphicsCmdList()->OMSetRenderTargets(m_renderTargetCount, &m_rtvHeapBegin, TRUE, &m_dsvHeapBegin);
}

void RenderTargetGroup::ClearRenderTargetView(uint32 p_index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rtvHeapBegin, p_index * m_rtvHeapSize);
	float clearColor[4] = { 0.3f,0.3f,0.3f,0.3f };
	m_graphicsCommandQueue->GetGraphicsCmdList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	//m_graphicsCommandQueue->GetGraphicsCmdList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	m_graphicsCommandQueue->GetGraphicsCmdList()->ClearDepthStencilView(m_dsvHeapBegin, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);
}

void RenderTargetGroup::ClearRenderTargetView()
{
	WaitResourceToTarget();

	for (uint32 i = 0; i < m_renderTargetCount; ++i)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rtvHeapBegin, i * m_rtvHeapSize);
		m_graphicsCommandQueue->GetGraphicsCmdList()->ClearRenderTargetView(rtvHandle, m_vecRenderTarget[i].clearColor, 0, nullptr);
	}

	m_graphicsCommandQueue->GetGraphicsCmdList()->ClearDepthStencilView(m_dsvHeapBegin, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);
}

void RenderTargetGroup::WaitTargetToResource()
{
	m_graphicsCommandQueue->GetGraphicsCmdList()->ResourceBarrier(m_renderTargetCount, m_targetToResource);
}

void RenderTargetGroup::WaitResourceToTarget()
{
	m_graphicsCommandQueue->GetGraphicsCmdList()->ResourceBarrier(m_renderTargetCount, m_resourceToTarget);
}

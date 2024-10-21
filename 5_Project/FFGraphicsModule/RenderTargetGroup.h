#pragma once

class Device;
class GraphicsCommandQueue;
class Texture;

class RenderTargetGroup
{
public:
	~RenderTargetGroup();

	void Create(RENDER_TARGET_TYPE p_type, vector<RenderTarget>& p_vecRT, Texture* p_dsTexture, Device* p_device, GraphicsCommandQueue* p_graphicsCommandQueue);

	void OMSetRenderTargets(uint32 p_count, uint32 p_offset);
	void OMSetRenderTargets();

	void ClearRenderTargetView(uint32 p_index);
	void ClearRenderTargetView();

	Texture* GetRTTexture(uint32 p_index)	{ return m_vecRenderTarget[p_index].texture; }
	Texture* GetDSTexture() { return m_dsTexture; }
	uint8 GetRenderTargetCount() { return m_renderTargetCount; }

	void WaitTargetToResource();
	void WaitResourceToTarget();

private:
	RENDER_TARGET_TYPE m_groupType;
	vector<RenderTarget> m_vecRenderTarget;
	uint8 m_renderTargetCount=0;
	Texture* m_dsTexture;
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;

private:
	uint32 m_rtvHeapSize;
	D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHeapBegin;
	D3D12_CPU_DESCRIPTOR_HANDLE m_dsvHeapBegin;

private:
	D3D12_RESOURCE_BARRIER m_targetToResource[8];
	D3D12_RESOURCE_BARRIER m_resourceToTarget[8];

private:
	// 참조용 변수들
	Device* m_device;
	GraphicsCommandQueue* m_graphicsCommandQueue;

};


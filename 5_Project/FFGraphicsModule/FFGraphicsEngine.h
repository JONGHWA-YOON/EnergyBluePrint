#pragma once

#include "IFFGraphics.h"

class Device;
class GraphicsCommandQueue;
class ComputeCommandQueue;
class SwapChain;
class RootSignature;
class GraphicsDescriptorHeap;
class ComputeDescriptorHeap;
class ConstantBuffer;
class RenderTargetGroup;
class Texture;

/// <summary>
/// 그래픽스 엔진
/// 전반적으로 D3D12에 관련된 것들을 Class로 묶어 놓은 상태이고, 그것을 멤버변수로 가지고 있다.
/// 
/// 2023.06.16 김성찬 게임인재원
/// </summary>

class FFGraphicsEngine : public IFFGraphics
{
public:
	~FFGraphicsEngine();

private:
	// 모든 멤버면수를 초기화 한다.
	void Init(HWND p_hWnd, int p_width, int p_height, bool p_windowed);

public:
#pragma region Getter
	Device* GetDevice() { return m_device; }
	GraphicsCommandQueue* GetGraphicsCmdQueue() { return m_graphicsCmdQueue; }
	ComputeCommandQueue* GetComputeCmdQueue() { return m_computeCmdQueue; }
	SwapChain* GetSwapChain() { return m_swapChain; }
	RootSignature* GetRootSignature() { return m_rootSignature; }
	GraphicsDescriptorHeap* GetGraphicsDescriptor() { return m_graphicsDescriptor; }
	ComputeDescriptorHeap* GetComputeDescriptor() { return m_computeDescriptor; }

	ConstantBuffer* GetConstantBuffer(CONSTANT_BUFFER_TYPE p_type) { return m_constantBuffers[static_cast<uint8>(p_type)]; }

	WindowInfo& GetWindow() { return	m_windowInfo; }

	RenderTargetGroup* GetRTGroup(RENDER_TARGET_TYPE p_type) { return m_renderTargetGroup[static_cast<uint8>(p_type)]; }

#pragma endregion

public:
	void ClearRenderTargetView(RENDER_TARGET_TYPE p_type);
	void OMSetRenderTargets(RENDER_TARGET_TYPE p_type);
	void WaitTargetToResource(RENDER_TARGET_TYPE p_type);

	void CreateStructuredBuffer(uint32 p_id, uint32 p_size, uint32 p_count);
	void PushComputeUAVData(uint32 p_id, UAV_REGISTER p_reg);
	void PushGraphicsData(uint32 p_id, SRV_REGISTER p_reg);
	void Dispatch(MATERIAL_DESC m_materialDesc, uint32 p_x, uint32 p_y, uint32 p_z);

	bool GetInstancingBuffer(uint64 p_instanceID);
	void PushInstancingBufferData(uint64 p_id,InstancingParams& p_data);
	void ClearInstancingBuffer();

	void DrawFinal();

private:
	// 그래픽스 엔진에서 미리 사용될 상수 버퍼를 다 만든다.
	void CreateConstantBuffer(CBV_REGISTER p_reg, uint32 p_bufferSize, uint32 p_count);

	void CreateRenderTargetGroup();

	void DrawDeferredDebugInfo();

private:
	// 그려질 화면 크기 관련
	WindowInfo		m_windowInfo;
	D3D12_VIEWPORT	m_viewPort = {};
	// 특정 픽셀들을 선별(culling)하는 용도로 쓰임
	D3D12_RECT		m_scissorRect = {};

	Device* m_device;
	GraphicsCommandQueue* m_graphicsCmdQueue;
	ComputeCommandQueue* m_computeCmdQueue;
	SwapChain* m_swapChain;
	RootSignature* m_rootSignature;
	GraphicsDescriptorHeap* m_graphicsDescriptor;
	ComputeDescriptorHeap* m_computeDescriptor;
	Texture* m_dsTexture;
	vector<ConstantBuffer*> m_constantBuffers;

	array<RenderTargetGroup*, RENDER_TARGET_TYPE_COUNT> m_renderTargetGroup;

	// 디퍼드 디버깅을 출력할건지에 대한 불 값
	bool m_isDrawDefereedDebug = false;

public:
	// IGraphics을(를) 통해 상속됨
	virtual void Init(long long p_hWnd, float p_width, float p_height, bool p_isWindowed) override;
	virtual void RenderBegin(float p_deltaTime) override;
	virtual void RenderEnd() override;
	virtual void DrawMesh(const FFMatrix& p_WTM, const wstring& p_mesh, const FFMATERIAL_DESC& p_material, wstring& p_materialName, int p_materialIndex) override;
	//virtual void DrawSkinnedMesh(const unsigned __int64 p_id, const std::wstring& p_currentAnimationName ,const FF::FFMatrix& p_WTM, const std::wstring& p_mesh, const FF::FFMATERIAL_DESC& p_material, std::wstring& p_materialName) override;
	virtual void DrawParticle(const FFMatrix& p_WTM, const wstring& p_mesh, const FFMATERIAL_DESC& p_material) override;
	virtual void DrawDeferredDebug() override;
	virtual void SetCameraInfo(unsigned __int32 p_cameraIndex, float p_naer, float p_far, float p_fov, float p_scale, float p_width, float p_height, const FFMatrix& p_WTM) override;
	virtual void SetLightData(void* p_buffer, unsigned __int32 p_size) override;
	virtual void ResizeWindow(__int32 p_width, __int32 p_height) override;
	virtual void CreateMesh(const std::wstring& p_path) override;
	virtual void CreateShader(const std::wstring& p_path, const FF::ShaderInfo& p_info)override;
	virtual void CreateTexture(const std::wstring& p_path)override;
	virtual void CreateMaterial(const std::wstring& p_path, const FF::FFMATERIAL_DESC& p_desc)override;
	virtual void LoadFBX(const std::wstring& p_path, const std::wstring& p_shaderPath) override;
	virtual std::vector<FF::FFMeshData>& GetMeshDataFromFBX(const std::wstring& p_path) override;
	virtual void PushAnimationData(unsigned __int64 p_id, __int32 p_frame, __int32 p_nextFrame, float p_frameRatio) override;

	virtual void DrawSkinnedMesh(unsigned __int64 p_id, __int32 p_frame, __int32 p_nextFrame, float p_frameRatio, const std::wstring& p_currentAnimationName, const FF::FFMatrix& p_WTM, const std::wstring& p_mesh, const FF::FFMATERIAL_DESC& p_material, std::wstring& p_materialName,int p_materialIndex) override;

	// 에디터에 제공할 GPU_DESCRIPTOR_HANDLE값
	virtual unsigned __int64 GetGPUHandle() override;
};

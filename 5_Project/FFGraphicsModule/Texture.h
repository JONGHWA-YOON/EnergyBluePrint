#pragma once

#include "IResource.h"

class Device;
class GraphicsCommandQueue;

/// <summary>
/// 텍스쳐를 로드하는 클래스
/// 
/// 2023.06.16 김성찬 게임인재원
/// </summary>
class Texture : public IResource
{
public:
	Texture();
	virtual ~Texture();

	// 주어진 경로를 이용하여 텍스쳐 로드 및 View생성
	void Init(const wstring& p_path, Device* p_device, GraphicsCommandQueue* p_graphicsCommandQueue);

#pragma region Getter
	ComPtr<ID3D12Resource> GetTex2D() { return m_tex2D; }
	ComPtr<ID3D12DescriptorHeap> GetSRV() { return	m_srvHeap; }
	ComPtr<ID3D12DescriptorHeap> GetRTV() { return	m_rtvHeap; }
	ComPtr<ID3D12DescriptorHeap> GetDSV() { return	m_dsvHeap; }
	ComPtr<ID3D12DescriptorHeap> GetUAV() { return	m_uavHeap; }

	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCpuHandle() { return m_srvHeapBegin; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetUAVCpuHandle() { return m_uavHeapBegin; }

	float GetWidth() { return static_cast<float>(m_desc.Width); }
	float GetHeight() { return static_cast<float>(m_desc.Height); }
#pragma endregion

public:
	// 이미지 파일 로드하고 Resource생성
	void Load(const wstring& p_path);

	void Create(Device* p_device, GraphicsCommandQueue* p_graphicsCommandQueue, DXGI_FORMAT p_format, uint32 p_width, uint32 p_height,
		const D3D12_HEAP_PROPERTIES& p_heapProperty, D3D12_HEAP_FLAGS p_heapFlags,
		D3D12_RESOURCE_FLAGS p_resFlags, FFVec4 p_clearColor
		= FFVec4());

	void CreateFromResource(ComPtr<ID3D12Resource> p_tex2D, Device* p_device, GraphicsCommandQueue* p_graphicsCommandQueue);

private:
	ScratchImage			 		m_image;
	D3D12_RESOURCE_DESC				m_desc;
	ComPtr<ID3D12Resource>			m_tex2D;
									
	ComPtr<ID3D12DescriptorHeap>	m_srvHeap;
	ComPtr<ID3D12DescriptorHeap>	m_rtvHeap;
	ComPtr<ID3D12DescriptorHeap>	m_dsvHeap;
	ComPtr<ID3D12DescriptorHeap>	m_uavHeap;

	D3D12_CPU_DESCRIPTOR_HANDLE		m_srvHeapBegin = {};
	D3D12_CPU_DESCRIPTOR_HANDLE		m_uavHeapBegin = {};

private:
	// 참조용 변수들
	Device* m_device;
	GraphicsCommandQueue* m_graphicsCommandQueue;
};


#include "GraphicsPch.h"
#include "Texture.h"

//#include "FFGraphicsEngine.h"
#include "Device.h"
#include "CommandQueue.h"

Texture::Texture()
	: IResource(RESOURCE_TYPE::TEXTURE)
{

}

Texture::~Texture()
{

}

void Texture::Init(const wstring& p_path, Device* p_device, GraphicsCommandQueue* p_graphicsCommandQueue)
{
	m_device = p_device;
	m_graphicsCommandQueue = p_graphicsCommandQueue;
	Load(p_path);
}

void Texture::Load(const wstring& p_path)
{
	// 파일 확장자 얻기
	wstring ext = fs::path(p_path).extension();

	if (ext == L".dds" || ext == L".DDS")
	{
		::LoadFromDDSFile(p_path.c_str(), DDS_FLAGS_NONE, nullptr, m_image);
	}
	else if (ext == L".tga" || ext == L".TGA")
	{
		::LoadFromTGAFile(p_path.c_str(), nullptr, m_image);
	}
	else // png, jpg, jpeg, bmp
	{
		::LoadFromWICFile(p_path.c_str(), WIC_FLAGS_NONE, nullptr, m_image);
	}

	HRESULT hr = ::CreateTexture(m_device->GetDevice().Get(), m_image.GetMetadata(), &m_tex2D);
	if (FAILED(hr))
	{
		assert(nullptr);
	}

	vector<D3D12_SUBRESOURCE_DATA> subResources;

	hr = ::PrepareUpload(m_device->GetDevice().Get(),
		m_image.GetImages(),
		m_image.GetImageCount(),
		m_image.GetMetadata(),
		subResources);

	if (FAILED(hr))
	{
		assert(nullptr);
	}

	m_desc = m_tex2D->GetDesc();

	const uint64 bufferSize = ::GetRequiredIntermediateSize(m_tex2D.Get(), 0, static_cast<uint32>(subResources.size()));

	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	ComPtr<ID3D12Resource> textureUploadHeap;
	hr = m_device->GetDevice()->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(textureUploadHeap.GetAddressOf()));

	if (FAILED(hr))
	{
		assert(nullptr);
	}

	::UpdateSubresources(m_graphicsCommandQueue->GetResourceCmdList().Get(),
		m_tex2D.Get(),
		textureUploadHeap.Get(),
		0, 0,
		static_cast<unsigned int>(subResources.size()),
		subResources.data());

	m_graphicsCommandQueue->FlushResourceCommandQueue();

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	m_device->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_srvHeap));

	m_srvHeapBegin = m_srvHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = m_image.GetMetadata().format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = 1;
	m_device->GetDevice()->CreateShaderResourceView(m_tex2D.Get(), &srvDesc, m_srvHeapBegin);
}

void Texture::Create(Device* p_device, GraphicsCommandQueue* p_graphicsCommandQueue, DXGI_FORMAT p_format, uint32 p_width, uint32 p_height, const D3D12_HEAP_PROPERTIES& p_heapProperty, D3D12_HEAP_FLAGS p_heapFlags, D3D12_RESOURCE_FLAGS p_resFlags, FFVec4 p_clearColor /*= Vec4()*/)
{
	m_device = p_device;
	m_graphicsCommandQueue = p_graphicsCommandQueue;

	m_desc = CD3DX12_RESOURCE_DESC::Tex2D(p_format, p_width, p_height);
	m_desc.Flags = p_resFlags;

	D3D12_CLEAR_VALUE optimizedClearValue = {};
	D3D12_CLEAR_VALUE* pOptimizedClearValue = nullptr;
	D3D12_RESOURCE_STATES resourceStates = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON;

	if (p_resFlags & D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
	{
		resourceStates = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_DEPTH_WRITE;
		optimizedClearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);
		pOptimizedClearValue = &optimizedClearValue;
	}
	else if (p_resFlags & D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
	{
		resourceStates = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON;
		float clearColor[4] = { p_clearColor.x,p_clearColor.y,p_clearColor.z,p_clearColor.w };
		optimizedClearValue = CD3DX12_CLEAR_VALUE(p_format, clearColor);
		pOptimizedClearValue = &optimizedClearValue;
	}

	// Texture2D 생성
	HRESULT hr = m_device->GetDevice()->CreateCommittedResource(
		&p_heapProperty,
		p_heapFlags,
		&m_desc,
		resourceStates,
		pOptimizedClearValue,
		IID_PPV_ARGS(&m_tex2D)
	);

	assert(SUCCEEDED(hr));

	CreateFromResource(m_tex2D, m_device, m_graphicsCommandQueue);

}

void Texture::CreateFromResource(ComPtr<ID3D12Resource> p_tex2D,Device* p_device, GraphicsCommandQueue* p_graphicsCommandQueue)
{
	m_device = p_device;
	m_graphicsCommandQueue = p_graphicsCommandQueue;

	m_tex2D = p_tex2D;
	m_desc = m_tex2D->GetDesc();

	if (m_desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		heapDesc.NumDescriptors = 1;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		heapDesc.NodeMask = 0;

		m_device->GetDevice()->CreateDescriptorHeap(&heapDesc,IID_PPV_ARGS(&m_dsvHeap));

		D3D12_CPU_DESCRIPTOR_HANDLE DSVHandle = m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
		m_device->GetDevice()->CreateDepthStencilView(m_tex2D.Get(), nullptr, DSVHandle);
	}
	else
	{
		if (m_desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
		{
			D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
			heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			heapDesc.NumDescriptors = 1;
			heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			heapDesc.NodeMask = 0;

			m_device->GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_rtvHeap));

			D3D12_CPU_DESCRIPTOR_HANDLE RTVHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
			m_device->GetDevice()->CreateRenderTargetView(m_tex2D.Get(), nullptr, RTVHandle);
		}

		if (m_desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS)
		{
			// UAV
			D3D12_DESCRIPTOR_HEAP_DESC uavHeapDesc = {};
			uavHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			uavHeapDesc.NumDescriptors = 1;
			uavHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			uavHeapDesc.NodeMask = 0;
			m_device->GetDevice()->CreateDescriptorHeap(&uavHeapDesc, IID_PPV_ARGS(&m_uavHeap));

			m_uavHeapBegin = m_uavHeap->GetCPUDescriptorHandleForHeapStart();

			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
			uavDesc.Format = m_image.GetMetadata().format;
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

			m_device->GetDevice()->CreateUnorderedAccessView(m_tex2D.Get(), nullptr, &uavDesc, m_uavHeapBegin);
		}

		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = 1;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		m_device->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_srvHeap));

		m_srvHeapBegin = m_srvHeap->GetCPUDescriptorHandleForHeapStart();

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = m_image.GetMetadata().format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		m_device->GetDevice()->CreateShaderResourceView(m_tex2D.Get(), &srvDesc, m_srvHeapBegin);
	}
}

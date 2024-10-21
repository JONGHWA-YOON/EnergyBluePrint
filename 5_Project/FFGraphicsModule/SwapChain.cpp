#include "GraphicsPch.h"
#include "SwapChain.h"

//#include "FFGraphicsEngine.h"
#include "Device.h"
#include "CommandQueue.h"

void SwapChain::Init(Device* p_device, GraphicsCommandQueue* p_graphicsCommandQueue, const WindowInfo& p_info)
{
	m_device = p_device;
	m_graphicsCommandQueue = p_graphicsCommandQueue;
	CreateSwapChain(p_info);
}

void SwapChain::Present()
{
	m_swapChain->Present(1, 0);
}

void SwapChain::SwapIndex()
{
	m_backBufferIndex = (m_backBufferIndex + 1) % SWAP_CHAIN_BUFFER_COUNT;
}

void SwapChain::CreateSwapChain(const WindowInfo& p_info)
{
	// 이전에 만든 정보를 날림
	m_swapChain.Reset();

	// DXGI_SWAP_CHAIN_DESC 구조체 안에 DXGI_MODE_DESC구조체가 BufferDesc로 들어가 있음
	DXGI_SWAP_CHAIN_DESC _sd;
	_sd.BufferDesc.Width = static_cast<uint32>(p_info.width);	// 버퍼의 해상도 너비
	_sd.BufferDesc.Height = static_cast<uint32>(p_info.height);	// 버퍼의 해상도 높이
	_sd.BufferDesc.RefreshRate.Numerator = 60;	// 화면 갱신 비율
	_sd.BufferDesc.RefreshRate.Denominator = 1;	// 화면 갱신 비율
	_sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 버퍼의 디스플레이 형식
	_sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	_sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	_sd.SampleDesc.Count = 1; // 멀티 샘플링 OFF
	_sd.SampleDesc.Quality = 0;
	_sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 후면 버퍼에 렌더링 할 것
	_sd.BufferCount = SWAP_CHAIN_BUFFER_COUNT;	// 전면 + 후면
	_sd.OutputWindow = p_info.hwnd;
	_sd.Windowed = p_info.windowed;
	_sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	// 전면 후면 버퍼 교체 시 이전 프레임 정보 버림
	_sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	m_device->GetDXGI()->CreateSwapChain(
		m_graphicsCommandQueue->GetGraphicsCmdQueue().Get(), &_sd, &m_swapChain);
}

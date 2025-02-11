#include "MZDX11Renderer.h"
#include "Effects.h"
#include "DXTKFont.h"
#include "MZCamera.h"
#include "DeferredRenderer.h"
//#include "GridBuilder.h"

#include "XLParticleEffect.h"
#include "XLParticleSystem.h"

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "dxgi.lib" )	// adapter info

#pragma comment( lib, "XLParticleSystem.lib")

MZRenderer::LazyObjects<MZDX11Renderer> MZDX11Renderer::Instance;

MZ3DAPI::I3DRenderer* CreateDX11GraphicsInstance()
{
	return new MZDX11Renderer();
	//return Instance.Get();
}


void ReleaseDX11GraphicsInstance(MZ3DAPI::I3DRenderer* instance)
{
	delete instance;
}


MZDX11Renderer::MZDX11Renderer()
	: m_hWnd(0), m_screenWidth(1920), m_screenHeight(1080), m_4xMsaaQuality(0), m_enable4xMsaa(false),
	m_d3dDriverType(D3D_DRIVER_TYPE_HARDWARE), m_pFont(nullptr), switcher(0)
{
	//ZeroMemory(&m_viewPort, sizeof(D3D11_VIEWPORT));

	m_pFont = new DXTKFont();
	m_pCamera = new MZCamera();
	m_pCamera->SetMain();
	m_pCamera->LookAt(XMFLOAT3(8.0f, 8.0f, -8.0f), XMFLOAT3(0, 0, 0), XMFLOAT3(0, 1.0f, 0));
}

MZDX11Renderer::~MZDX11Renderer()
{
	RasterizerState::DestroyRenderStates();
	InputLayouts::DestroyAll();
	Effects::DestroyAll();

	SafeDelete(m_pCamera);
	SafeDelete(m_pFont);

	//ReleaseCOM(m_depthStencilView);
	//ReleaseCOM(m_swapChain);
	//ReleaseCOM(m_depthStencilBuffer);

	if (m_d3dImmediateContext)
	{
		m_d3dImmediateContext->ClearState();
	}

	ReleaseCOM(m_d3dImmediateContext);
	ReleaseCOM(m_d3dDevice);

}


bool MZDX11Renderer::Initialize()
{
	// 디버그 모드인 경우 DX11 디버그 모드
	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// 디바이스와 디바이스 컨텍스트 생성
	HRESULT hr = D3D11CreateDevice(0, m_d3dDriverType, 0, createDeviceFlags, 0, 0,
		D3D11_SDK_VERSION, &m_d3dDevice, &m_featureLevel, &m_d3dImmediateContext);
	if (FAILED(hr))
	{
		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
		return false;
	}
	if (m_featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
		return false;
	}

	// 멀티샘플링 체크
	HR(m_d3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_4xMsaaQuality));
	assert(m_4xMsaaQuality > 0);

	// Effects
	/// 입력 배치(InputLayout)가 쉐이더 시그니처(sementics/의미소)에 의존하기 때문에, 이펙트를 먼저 초기화해야 함.
	// Must init Effects first since InputLayouts depend on shader signatures.
	Effects::InitAll(m_d3dDevice.Get());
	InputLayouts::InitAll(m_d3dDevice.Get());
	RasterizerState::CreateRenderStates(m_d3dDevice.Get());

	// 폰트
	m_pFont->Initialize(m_d3dDevice.Get(), (TCHAR*)L"Font/gulim9k.spritefont");
	m_pFont->SetLineSpacing(12.0f);
	m_fontLineSpace = m_pFont->GetLineSpacing();

	//m_grid = new Grid(m_d3dDevice.Get(), m_d3dImmediateContext.Get(), m_wireframeRS);
	//m_grid->Initialize();

	// Construct Objects
	// 그리드
	/// 빌더패턴?
	/// 그런데 이 방식이 new Grid(ID3D11Device*, ID3D11DeviceContext*, ID3D11RasterizerState*)에
	/// 비해 더 나은 점이 무엇일까?
	/*IRenderableObject* grid = (new GridBuilder())->
		SetDevice(m_d3dDevice.Get())->
		SetDeviceContext(m_d3dImmediateContext.Get())->
		SetRasterizerState(m_wireframeRS)->
		Build();

	objectsList.emplace_back(grid);*/

	GetAdapterInfo();


	/// Particle System 관련
	//m_pParticleSystem = new XLParticleSystem();
	//m_pParticleSystem->Init(m_d3dDevice.Get(), m_d3dImmediateContext.Get(), 5000);

	//FireParticleEffect* fire = new FireParticleEffect();

	//m_pParticleSystem->AddParticleEffect(fire);


	return true;
}

void MZDX11Renderer::SetOutputWindow(unsigned int hWnd)
{
	m_hWnd = (HWND)hWnd;
	RECT rect;
	GetWindowRect(m_hWnd, &rect);
	m_screenWidth = rect.right - rect.left;
	m_screenHeight = rect.bottom - rect.top;

	// Fill out a DXGI_SWAP_CHAIN_DESC
	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = m_screenWidth;
	sd.BufferDesc.Height = m_screenHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	if (m_enable4xMsaa)
	{
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = m_4xMsaaQuality - 1;
	}
	else
	{
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = m_hWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	IDXGIDevice* dxgiDevice = 0;
	HR(m_d3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

	IDXGIAdapter* dxgiAdapter = 0;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

	IDXGIFactory* dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

	HR(dxgiFactory->CreateSwapChain(m_d3dDevice.Get(), &sd, &m_swapChain));

	dxgiDevice->Release();
	dxgiAdapter->Release();
	dxgiFactory->Release();

	OnResize();

	DeferredRenderer::Instance.Get().Initialize(m_d3dDevice.Get(), m_d3dImmediateContext.Get(), m_screenWidth, m_screenHeight);
}

void MZDX11Renderer::Update(float deltaTime)
{
	m_deltaTime = deltaTime;

	if (GetAsyncKeyState('O') & 0x0001)
	{
		if (switcher == 1)
		{
			switcher = 0;
		}
		else if (switcher == 0)
		{
			switcher = 1;
		}
	}

	m_cameraSpeed = 1.0f;
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		m_cameraSpeed = 2.0f;
	}

	// 카메라 이동
	/*if (GetAsyncKeyState('W') & 0x8000)
	{
		m_pCamera->Walk(10.0f * deltaTime * m_cameraSpeed);
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		m_pCamera->Strafe(-10.0f * deltaTime * m_cameraSpeed);
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		m_pCamera->Walk(-10.0f * deltaTime * m_cameraSpeed);
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		m_pCamera->Strafe(10.0f * deltaTime * m_cameraSpeed);
	}
	if (GetAsyncKeyState('Q') & 0x8000)
	{
		m_pCamera->WorldUpDown(-10.0f * deltaTime * m_cameraSpeed);
	}
	if (GetAsyncKeyState('E') & 0x8000)
	{
		m_pCamera->WorldUpDown(10.0f * deltaTime * m_cameraSpeed);
	}*/

	//m_pCamera->UpdateViewMatrix();

	//m_grid->Update(XMMatrixIdentity(), m_pCamera->View(), m_pCamera->Proj());
	DeferredRenderer::Instance.Get().Update(MZCamera::GetMainCamera(), deltaTime);


	/// Particle System 관련
	//m_pParticleSystem->Update(deltaTime, 0, nullptr);

}


void MZDX11Renderer::BeginRender()
{
	assert(m_d3dImmediateContext);

	DeferredRenderer::Instance.Get().SwapBackBuffer();
	m_backBufferTexture = DeferredRenderer::Instance.Get().GetBackBufferTexture();
	HR(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(m_backBufferTexture.GetAddressOf())));

	//m_d3dImmediateContext->OMSetRenderTargets(1, m_backBufferRTV.GetAddressOf(), m_depthStencilView.Get());
	//m_d3dImmediateContext->ClearRenderTargetView(m_backBufferRTV.Get(), reinterpret_cast<const float*>(&DirectX::Colors::Black));
	//m_d3dImmediateContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}


void MZDX11Renderer::Render()
{
	BeginRender();

	/// Particle System 관련
	//m_pParticleSystem->Render();

	DeferredRenderer::Instance.Get().RenderToBackBuffer();

	// 피처레벨, 어댑터 상태 등 출력
	if(switcher == 1)
		DrawStatus();

	m_d3dImmediateContext->RSSetState(0);

	EndRender();
}


void MZDX11Renderer::EndRender()
{
	//m_d3dImmediateContext->Flush();
	//assert(m_swapChain);
	if(m_swapChain)
		HR(m_swapChain->Present(0, 0));
}


void MZDX11Renderer::Finalize()
{

}


void MZDX11Renderer::ResizeResolution(unsigned int width, unsigned int height)
{
	m_screenWidth = width;
	m_screenHeight = height;
	OnResize();
}

void MZDX11Renderer::OnResize()
{
	assert(m_d3dImmediateContext);
	assert(m_d3dDevice);
	assert(m_swapChain);

	//ReleaseCOM(m_backBufferRTV);

	//ReleaseCOM(m_depthStencilView);
	//ReleaseCOM(m_depthStencilBuffer);

	//swap chain resize & recreate render target view
	HR(m_swapChain->ResizeBuffers(1, m_screenWidth, m_screenHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	D3D11_TEXTURE2D_DESC bTextureDescription
	{
		.Width = (UINT)m_screenWidth,
		.Height = (UINT)m_screenHeight,
		.MipLevels = 1,
		.ArraySize = 1,
		.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
		.SampleDesc{.Count = 1, .Quality = 0 },
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
		.CPUAccessFlags = 0,
		.MiscFlags = 0,
	};

	HR(m_d3dDevice->CreateTexture2D(&bTextureDescription, NULL, m_backBufferTexture.GetAddressOf()));
	/*ID3D11Texture2D* backBuffer;
	HR(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
	HR(m_d3dDevice->CreateRenderTargetView(backBuffer, 0, &m_backBufferRTV));
	backBuffer->Release();*/

	// create depth stencil view
	//D3D11_TEXTURE2D_DESC depthStencilDesc;
	//depthStencilDesc.Width = m_screenWidth;
	//depthStencilDesc.Height = m_screenHeight;
	//depthStencilDesc.MipLevels = 1;
	//depthStencilDesc.ArraySize = 1;
	//depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//if (m_enable4xMsaa)
	//{
	//	depthStencilDesc.SampleDesc.Count = 4;
	//	depthStencilDesc.SampleDesc.Quality = m_4xMsaaQuality - 1;
	//}
	//else
	//{
	//	depthStencilDesc.SampleDesc.Count = 1;
	//	depthStencilDesc.SampleDesc.Quality = 0;
	//}

	//depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	//depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	//depthStencilDesc.CPUAccessFlags = 0;
	//depthStencilDesc.MiscFlags = 0;

	//HR(m_d3dDevice->CreateTexture2D(&depthStencilDesc, 0, &m_depthStencilBuffer));
	//HR(m_d3dDevice->CreateDepthStencilView(m_depthStencilBuffer.Get(), 0, &m_depthStencilView));

	// bind render target view and depth/stencil view
	//m_d3dImmediateContext->OMSetRenderTargets(1, m_backBufferRTV.GetAddressOf(), m_depthStencilView.Get());

	// set the viewport transform
	/*m_viewPort.TopLeftX = 0;
	m_viewPort.TopLeftY = 0;
	m_viewPort.Width = static_cast<float>(m_screenWidth);
	m_viewPort.Height = static_cast<float>(m_screenHeight);
	m_viewPort.MinDepth = 0.0f;
	m_viewPort.MaxDepth = 1.0f;

	m_d3dImmediateContext->RSSetViewports(1, &m_viewPort);*/

	// 투영 행렬 재계산
	m_pCamera->SetFrustum(0.25f * MathHelper::Pi, GetAspectRatio(), 1.0f, 1000.0f);
}


void MZDX11Renderer::OnMouseDown(int btnState, int x, int y)
{
	if ((btnState & MK_RBUTTON) != 0)
	{
		m_lastMousePos.x = x;
		m_lastMousePos.y = y;

		SetCapture(m_hWnd);
	}
	else if ((btnState & MK_LBUTTON) != 0)
	{
		Pick(x, y);
	}
}


void MZDX11Renderer::OnMouseUp(int x, int y)
{
	ReleaseCapture();
}


void MZDX11Renderer::OnMouseMove(int btnState, int x, int y)
{
	if ((btnState & MK_RBUTTON) != 0)
	{
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - m_lastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - m_lastMousePos.y));

		m_pCamera->Pitch(dy);
		m_pCamera->RotateY(dx);
	}

	m_lastMousePos.x = x;
	m_lastMousePos.y = y;
}

IRenderableObject* MZDX11Renderer::Pick(float normalizedX, float normalizedY)
{
	return DeferredRenderer::Instance.Get().Pick(MZCamera::GetMainCamera(), normalizedX, normalizedY);
}

HRESULT MZDX11Renderer::GetAdapterInfo()
{
	IDXGIAdapter1* pAdapter;
	IDXGIFactory1* pFactory = NULL;

	HRESULT hr = S_OK;

	if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&pFactory)))
	{
		return E_FAIL;
	}

	hr = pFactory->EnumAdapters1(0, &pAdapter);

	if (hr != DXGI_ERROR_NOT_FOUND)
	{
		pAdapter->GetDesc1(&m_AdapterDesc1);
	}

	pAdapter->Release();
	pFactory->Release();

	return S_OK;
}


void MZDX11Renderer::DrawStatus()
{
	DirectX::XMVECTORF32 _color = { 1.f, 1.f, 1.f, 1.f };

	// FPS, DeltaTime
	static float accumulatedTime = 0.0f;
	static float fps = 0.0f;
	static float deltaTimeMS = 0.0f;

	// 측정 주기는 0.2초
	if (accumulatedTime > 0.2f)
	{
		fps = 1.0f / m_deltaTime;
		deltaTimeMS = m_deltaTime * 1000.0f;
		accumulatedTime = 0.0f;
	}
	accumulatedTime += m_deltaTime;
	m_pFont->DrawText(0, 10, 1.0f, _color, (TCHAR*)L"FPS : %.2f", fps);
	m_pFont->DrawText(0, 10 + m_fontLineSpace, 1.0f, _color, (TCHAR*)L"DeltaTime : %.4f ms", deltaTimeMS);

	// 피쳐레벨
	int _yPos = 50;
	m_pFont->DrawText(0, _yPos, 1.0f, _color, (TCHAR*)L"Feature Level : %x", m_featureLevel);

	// 어댑터 정보
	m_pFont->DrawText(0, _yPos += m_fontLineSpace, 1.0f, _color, (TCHAR*)L"Description: %s", m_AdapterDesc1.Description);
	m_pFont->DrawText(0, _yPos += m_fontLineSpace, 1.0f, _color, (TCHAR*)L"VendorID: %u", m_AdapterDesc1.VendorId);
	m_pFont->DrawText(0, _yPos += m_fontLineSpace, 1.0f, _color, (TCHAR*)L"DeviceID: %u", m_AdapterDesc1.DeviceId);
	m_pFont->DrawText(0, _yPos += m_fontLineSpace, 1.0f, _color, (TCHAR*)L"SubSysID: %u", m_AdapterDesc1.SubSysId);
	m_pFont->DrawText(0, _yPos += m_fontLineSpace, 1.0f, _color, (TCHAR*)L"Revision: %u", m_AdapterDesc1.Revision);
	m_pFont->DrawText(0, _yPos += m_fontLineSpace, 1.0f, _color, (TCHAR*)L"VideoMemory: %lu MB", m_AdapterDesc1.DedicatedVideoMemory / 1024 / 1024);
	m_pFont->DrawText(0, _yPos += m_fontLineSpace, 1.0f, _color, (TCHAR*)L"SystemMemory: %lu MB", m_AdapterDesc1.DedicatedSystemMemory / 1024 / 1024);
	m_pFont->DrawText(0, _yPos += m_fontLineSpace, 1.0f, _color, (TCHAR*)L"SharedSysMemory: %lu MB", m_AdapterDesc1.SharedSystemMemory / 1024 / 1024);
	m_pFont->DrawText(0, _yPos += m_fontLineSpace, 1.0f, _color, (TCHAR*)L"AdpaterLuid: %u.%d", m_AdapterDesc1.AdapterLuid.HighPart, m_AdapterDesc1.AdapterLuid.LowPart);

	// 클라이언트 영역의 해상도
	m_pFont->DrawText(0, _yPos += 2 * m_fontLineSpace, 1.0f, _color, (TCHAR*)L"Client Resolution: %dx%d", m_screenWidth, m_screenHeight);

	// 카메라 정보
	m_pFont->DrawText(0, _yPos += m_fontLineSpace, 1.0f, _color, (TCHAR*)L"Camera Pos : %.2f / %.2f / %.2f", m_pCamera->GetPosition().x, m_pCamera->GetPosition().y, m_pCamera->GetPosition().z);
}


float MZDX11Renderer::GetAspectRatio() const
{
	return static_cast<float>(m_screenWidth) / m_screenHeight;
}

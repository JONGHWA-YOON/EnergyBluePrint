#include "DeferredBuffers.h"
#include <ranges>

#include "d3dUtil.h"

// ----------------------------------------------------------------------------
LazyObject<DeferredBuffers> DeferredBuffers::SingleInstance;

DeferredBuffers::DeferredBuffers()
{
    for (int iIndex = 0; iIndex < GBUFFERTYPE::BUFFER_COUNT; iIndex++)
    {
        m_RenderTargetTextures[iIndex] = nullptr;
        m_RenderTargetViews[iIndex] = nullptr;
        m_ShaderResourceViews[iIndex] = nullptr;
    }

    m_GeomDepthBuffer = nullptr;
    m_GeomDepthView = nullptr;
    m_BackDepthBuffer = nullptr;
    m_BackDepthView = nullptr;
}

// ----------------------------------------------------------------------------

DeferredBuffers::~DeferredBuffers()
{
}

// ----------------------------------------------------------------------------

bool DeferredBuffers::Initialize(ID3D11Device* device,
    UINT iTextureWidth,
    UINT iTextureHeight,
    float fMaxDepth,
    float fMinDepth,
    int iMsaaCount,
    int iMsaaQuality)
{
    // Initialize the texture size
    m_iTextureHeight = iTextureHeight;
    m_iTextureWidth = iTextureWidth;

    // ----------------------------------------------------------------------------
    // 텍스처부터 디퍼드에 필요한 만큼 생성한다. 
    D3D11_TEXTURE2D_DESC gTextureDescription
    {
        .Width = m_iTextureWidth,
        .Height = m_iTextureHeight,
        .MipLevels = 1,
        .ArraySize = 1,
        .Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
        .SampleDesc{.Count = 1, .Quality = 0 },
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
        .CPUAccessFlags = 0,
        .MiscFlags = 0
    };
    for (int iTextureIndex = 0; iTextureIndex < GBUFFERTYPE::BUFFER_COUNT; iTextureIndex++)
        HR(device->CreateTexture2D(&gTextureDescription, NULL, m_RenderTargetTextures[iTextureIndex].GetAddressOf()));

    // 백버퍼 텍스처들을 필요한 만큼 생성한다.
    D3D11_TEXTURE2D_DESC bTextureDescription
    {
        .Width = m_iTextureWidth,
        .Height = m_iTextureHeight,
        .MipLevels = 1,
        .ArraySize = 1,
        .Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
        .SampleDesc{.Count = 1, .Quality = 0 },
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
        .CPUAccessFlags = 0,
        .MiscFlags = D3D11_RESOURCE_MISC_SHARED,
    };

    for (unsigned int iTextureIndex = 0; iTextureIndex < BACK_BUFFER_COUNT; iTextureIndex++)
    {
        HR(device->CreateTexture2D(&bTextureDescription, NULL, m_BackRenderTargetTextures[iTextureIndex].GetAddressOf()));
        IDXGIResource* dxgiResource{ nullptr };
        m_BackRenderTargetTextures[iTextureIndex]->QueryInterface<IDXGIResource>(&dxgiResource);
        dxgiResource->GetSharedHandle(&m_BackBufferSharedHandles[iTextureIndex]);
        dxgiResource->Release();
    }

    // ----------------------------------------------------------------------------
    // 텍스처마다 렌더타겟 뷰를 생성한다.
    // 렌더타겟 뷰는 밉맵의 갯수를 받는다.
    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDescription
    {
        .Format = gTextureDescription.Format,
        .ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D,
        .Texture2D{.MipSlice = 0}
    };

    for (int iTextureIndex = 0; iTextureIndex < GBUFFERTYPE::BUFFER_COUNT; iTextureIndex++)
        HR(device->CreateRenderTargetView(m_RenderTargetTextures[iTextureIndex].Get(),
            &renderTargetViewDescription, m_RenderTargetViews[iTextureIndex].GetAddressOf()));

    for (int iTextureIndex = 0; iTextureIndex < BACK_BUFFER_COUNT; iTextureIndex++)
        HR(device->CreateRenderTargetView(m_BackRenderTargetTextures[iTextureIndex].Get(),
            &renderTargetViewDescription, m_BackRenderTargetViews[iTextureIndex].GetAddressOf()));

    // ----------------------------------------------------------------------------
    // 텍스처마다 쉐이더 뷰를 생성한다.
    // 쉐이더 데스크립션의 경우, mipmap의 단계와 가장 디테일한 밉맵 단계를 받는다.
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDescription
    {
        .Format = gTextureDescription.Format,
        .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
        .Texture2D {.MostDetailedMip = 0, .MipLevels = 1, }
    };

    for (int iTextureIndex = 0; iTextureIndex < GBUFFERTYPE::BUFFER_COUNT; iTextureIndex++)
        HR(device->CreateShaderResourceView(m_RenderTargetTextures[iTextureIndex].Get(),
            &shaderResourceViewDescription,
            m_ShaderResourceViews[iTextureIndex].GetAddressOf()));

    // ----------------------------------------------------------------------------
    // 뎁스 스텐실 버퍼를 생성한다.
    D3D11_TEXTURE2D_DESC depthBufferDescription
    {
        .Width = m_iTextureWidth,
        .Height = m_iTextureHeight,
        .MipLevels = 1,
        .ArraySize = 1,
        .Format = DXGI_FORMAT_D24_UNORM_S8_UINT,
        .SampleDesc{.Count = 1, .Quality = 0},
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_DEPTH_STENCIL,
        .CPUAccessFlags = 0,
        .MiscFlags = 0,
    };
    HR(device->CreateTexture2D(&depthBufferDescription, NULL, &m_GeomDepthBuffer));
    HR(device->CreateTexture2D(&depthBufferDescription, NULL, &m_BackDepthBuffer));

    // 뎁스 스텐실 뷰를 생성한다.
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDescription
    {
        .Format = DXGI_FORMAT_D24_UNORM_S8_UINT,
        .ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D,
        .Flags = 0,
        .Texture2D{.MipSlice = 0},
    };
    HR(device->CreateDepthStencilView(m_GeomDepthBuffer.Get(),
        &depthStencilViewDescription,
        m_GeomDepthView.GetAddressOf()));
    HR(device->CreateDepthStencilView(m_BackDepthBuffer.Get(),
        &depthStencilViewDescription,
        m_BackDepthView.GetAddressOf()));

    // ----------------------------------------------------------------------------
    // 렌더타겟 텍스처의 화면 전체에 대한 뷰포트를 설정한다.
    m_Viewport.Width = (float)m_iTextureWidth;
    m_Viewport.Height = (float)m_iTextureHeight;
    m_Viewport.MinDepth = 0.0f;
    m_Viewport.MaxDepth = 1.0f;
    m_Viewport.TopLeftX = 0.0f;
    m_Viewport.TopLeftY = 0.0f;

    // 이 과정이 전부 성공하면 true를 반환
    return true;
}

void DeferredBuffers::Shutdown()
{
    // 디퍼드 버퍼가 갖고 있는 리소스들을 모두 릴리즈한다.
    // 어차피 comptr로 들고 있으니 죽을때 해제가 될 것이지만,
    // 런타임 도중 임의로 리소스들을 해제하고 싶을 때가 있을수도?
    // Release the render targets
    for (int iIndex = 0; iIndex < GBUFFERTYPE::BUFFER_COUNT; iIndex++)
    {
        if (m_RenderTargetTextures[iIndex].Get() != nullptr)
            m_RenderTargetTextures[iIndex]->Release();
        if (m_RenderTargetViews[iIndex].Get() != nullptr)
            m_RenderTargetViews[iIndex]->Release();
        if (m_ShaderResourceViews[iIndex].Get() != nullptr)
            m_ShaderResourceViews[iIndex]->Release();
    }

    // Release the depth/stencil target and view
    if (m_GeomDepthBuffer.Get() != nullptr)
        m_GeomDepthBuffer->Release();
    if (m_GeomDepthView.Get() != nullptr)
        m_GeomDepthView->Release();
    if (m_BackDepthBuffer.Get() != nullptr)
        m_BackDepthBuffer->Release();
    if (m_BackDepthView.Get() != nullptr)
        m_BackDepthView->Release();
}

// 디바이스 컨텍스트의 렌더타겟을 이 디퍼드 버퍼로 바꾼다.
void DeferredBuffers::RenderTargetGBuffers(ID3D11DeviceContext* deviceContext)
{
    deviceContext->OMSetRenderTargets(GBUFFER_COUNT, m_RenderTargetViews->GetAddressOf(), m_GeomDepthView.Get());
    deviceContext->RSSetViewports(1, &m_Viewport);
}
void DeferredBuffers::RenderTargetFrontBuffer(ID3D11DeviceContext* deviceContext)
{
    deviceContext->OMSetRenderTargets(1, m_BackRenderTargetViews[frontBufferIndex].GetAddressOf(), m_BackDepthView.Get());
}
//void DeferredBuffers::RenderTargetBuffer(ID3D11DeviceContext* deviceContext, GBUFFERTYPE bufferType)
//{
//    deviceContext->OMSetRenderTargets(1, m_RenderTagetViews[bufferType].GetAddressOf(), m_BackDepthView.Get());
//    deviceContext->RSSetViewports(1, &m_Viewport);
//}

// 렌더 타겟들을 모두 특정 색상으로 초기화한다.
void DeferredBuffers::ClearRenderTargets(ID3D11DeviceContext* deviceContext, float color[4])
{
    // 렌더 타겟 클리어
    for (unsigned int iTextureIndex = 0; iTextureIndex < GBUFFERTYPE::BUFFER_COUNT; iTextureIndex++)
        deviceContext->ClearRenderTargetView(m_RenderTargetViews[iTextureIndex].Get(), color);
    deviceContext->ClearRenderTargetView(m_BackRenderTargetViews[frontBufferIndex].Get(), color);
    // 뎁스 스텐실 뷰 클리어
    deviceContext->ClearDepthStencilView(m_GeomDepthView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
    deviceContext->ClearDepthStencilView(m_BackDepthView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}
void* DeferredBuffers::GetSharedBackBufferTextureHandle()
{
    return m_BackBufferSharedHandles[backBufferIndex];
}
void DeferredBuffers::SwapBackBuffer()
{
    backBufferIndex = frontBufferIndex;
    ++frontBufferIndex %= BACK_BUFFER_COUNT;
}


// ----------------------------------------------------------------------------
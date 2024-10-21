#pragma once

#include <d3d11.h>
#include <wrl.h>
#include "LazyObject.h"

// Render target type

// DeferredBuffers는 세 개의 텍스처 리소스들을 할당받고, 각 텍스처 리소스마다 렌더타겟 뷰, 쉐이더 리소스 뷰를 생성한다.
// Microsoft::WRL::ComPtr<ID3D11Texture2D>			m_RenderTargetTextures[BUFFERTYPE::GBUFFER_COUNT];
// Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RenderTagetViews[BUFFERTYPE::GBUFFER_COUNT];
// Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_ShaderResourceViews[BUFFERTYPE::GBUFFER_COUNT];
// Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pDepthStencilBuffer;
// Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
class DeferredBuffers
{
public:
    static constexpr unsigned int GBUFFER_COUNT = 4;
    static constexpr unsigned int BACK_BUFFER_COUNT = 2000;
    enum GBUFFERTYPE
    {
        gNormal,
        gDiffuseAlbedo,
        gTangent,
        gPositionH,
        FinalOutput,
        BUFFER_COUNT
    };
    ~DeferredBuffers();

    bool Initialize(ID3D11Device* device,
        UINT iTextureWidth,
        UINT iTextureHeight,
        float fMaxDepth,
        float fMinDepth,
        int iMsaaCount,
        int iMsaaQuality);
    void Shutdown();

    void RenderTargetGBuffers(ID3D11DeviceContext* deviceContext);
    void RenderTargetFrontBuffer(ID3D11DeviceContext* deviceContext);
    void ClearRenderTargets(ID3D11DeviceContext* deviceContext, float color[4]);
    void* GetSharedBackBufferTextureHandle();
    void SwapBackBuffer();

    inline ID3D11ShaderResourceView* GetShaderResourceView(int iRenderTargetIndex)
    {
        if (iRenderTargetIndex < GBUFFERTYPE::BUFFER_COUNT)
        {
            return m_ShaderResourceViews[iRenderTargetIndex].Get();
        }
        else
        {
            return nullptr;
        }
    }
    static LazyObject<DeferredBuffers> SingleInstance;
    friend LazyObject<DeferredBuffers>;
private:
    DeferredBuffers();
    UINT frontBufferIndex = 0;
    UINT backBufferIndex = 1;
    UINT m_iTextureWidth;
    UINT m_iTextureHeight;

    Microsoft::WRL::ComPtr<ID3D11Texture2D>			m_RenderTargetTextures[GBUFFERTYPE::BUFFER_COUNT];
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RenderTargetViews[GBUFFERTYPE::BUFFER_COUNT];
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_ShaderResourceViews[GBUFFERTYPE::BUFFER_COUNT];

    Microsoft::WRL::ComPtr<ID3D11Texture2D>			m_BackRenderTargetTextures[BACK_BUFFER_COUNT];
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_BackRenderTargetViews[BACK_BUFFER_COUNT];
    HANDLE m_BackBufferSharedHandles[BACK_BUFFER_COUNT]{ nullptr };
    //Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_BackShaderResourceViews[BACK_BUFFER_COUNT];

    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_GeomDepthBuffer;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_GeomDepthView;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_BackDepthBuffer;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_BackDepthView;

    D3D11_VIEWPORT m_Viewport;
};

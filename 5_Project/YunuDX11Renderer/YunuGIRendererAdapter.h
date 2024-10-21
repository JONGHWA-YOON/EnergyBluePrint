#pragma once
#include "IYunuGI3DRenderer.h"
#include "YunuDX11Renderer.h"
#include <d3d11.h>
#include <Windows.h>
#include <memory>

// YunuGI 인터페이스 중 Renderer에 인터페이스를 맞추는 어댑터
namespace yunuGIAdapter
{
    class RendererAdapter : public yunuGI::I3DRenderer
    {
    private:
        //YunuDX11Renderer YunuDX11Renderer::GetInstance();
    public:
        RendererAdapter(const yunuGI::I3DRendererDesc& desc) {
            YunuDX11Renderer::SingleInstance.Get().Initialize();
            YunuDX11Renderer::SingleInstance.Get().ResizeResolution(1024, 768);
        }
        virtual ~RendererAdapter() { YunuDX11Renderer::SingleInstance.Release(); }
        // DX를 초기화한다.
        //virtual bool Initialize() { return true; /*return YunuDX11Renderer::SingleInstance.Get().Initialize(); */ }

        // 그림을 그릴 대상 윈도우를 정함
        virtual void SetOutputWindow(unsigned long long int hWnd) { YunuDX11Renderer::SingleInstance.Get().SetOutputWindow(reinterpret_cast<HWND>(hWnd)); }
        virtual void Update(float deltaTime) { YunuDX11Renderer::SingleInstance.Get().Update(deltaTime); }
        // 렌더
        virtual void Render() {

            std::vector<D3D11_VIEWPORT> viewports{
                D3D11_VIEWPORT{ 0, 0, 1, 1}
            };
            /*std::vector<D3D11_VIEWPORT> viewports{
                D3D11_VIEWPORT{ 0,0,0.1,0.1}
            };*/
            //YunuDX11Renderer::SingleInstance.Get().BeginRender();
            YunuDX11Renderer::SingleInstance.Get().RenderToTexture(viewports);
            //YunuDX11Renderer::SingleInstance.Get().EndRender();
        };

        // 엔진을 종료한다.
        //virtual void Release() { YunuDX11Renderer::SingleInstance.Release(); };
        virtual void ResizeResolution(unsigned int width, unsigned int height) override
        {
            YunuDX11Renderer::SingleInstance.Get().ResizeResolution(width, height);
        }

        // 혹여나 내부 그래픽스 구조를 알아야 할 때
        virtual ID3D11Device* QueryD3d11Device() { return YunuDX11Renderer::SingleInstance.Get().md3dDevice; };
        virtual ID3D11DeviceContext* QueryD3dDeviceContext() { return YunuDX11Renderer::SingleInstance.Get().md3dDeviceContext; }
        virtual ID3D11ShaderResourceView* QueryD3D11RenderOutputSRV()
        {

            //return (ID3D11ShaderResourceView*)(sharedHandle);
            //return tempSrv;


            return DeferredBuffers::SingleInstance.Get().GetShaderResourceView(DeferredBuffers::GBUFFERTYPE::gNormal);
        }
        virtual void* QuerySharedOutputHandle()
        {
            return DeferredBuffers::SingleInstance.Get().GetSharedBackBufferTextureHandle();
        }
    };
}

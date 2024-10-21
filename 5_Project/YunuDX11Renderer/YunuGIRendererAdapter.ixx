#ifdef USE_MODULE
module;
#include "IYunuGI3DRenderer.h"
#include "YunuDX11Renderer.h"
#include <d3d11.h>

#include <Windows.h>
#include <memory>
export module YunuGIAdapter.RendererAdapter;

// YunuGI 인터페이스 중 Renderer에 인터페이스를 맞추는 어댑터
export namespace YunuGIAdapter
{
    class RendererAdapter : public YunuGI::I3DRenderer
    {
    private:
        //YunuDX11Renderer YunuDX11Renderer::GetInstance();
    public:
        virtual ~RendererAdapter() { YunuDX11Renderer::SingleInstance.Release(); }
        // DX를 초기화한다.
        virtual bool Initialize() { return YunuDX11Renderer::SingleInstance.Get().Initialize(); }

        // 그림을 그릴 대상 윈도우를 정함
        virtual void SetOutputWindow(HWND hWnd) { YunuDX11Renderer::SingleInstance.Get().SetOutputWindow(hWnd); }
        // 업데이트는 최종 빌드에서는 쓰지 않는다. 이 인터페이스는 오직 흔적기관으로서 존재하며, 빠른 시일 내에 삭제되어야 한다.
        virtual void Update(float deltaTime) { YunuDX11Renderer::SingleInstance.Get().Update(deltaTime); }
        // 렌더링 시작
        virtual void BeginRender() { YunuDX11Renderer::SingleInstance.Get().BeginRender(); };
        virtual void Render() {
            std::vector<D3D11_VIEWPORT> viewports{
                D3D11_VIEWPORT{ 0,0,1,1}
            };
            YunuDX11Renderer::SingleInstance.Get().Render(viewports);
        };

        // 렌더링 마무리. 그릴 것 다 그린 상태에서 Present
        virtual void EndRender() { YunuDX11Renderer::SingleInstance.Get().EndRender(); };

        // 엔진을 종료한다.
        virtual void Release() { YunuDX11Renderer::SingleInstance.Release(); };
        virtual void OnMouseDown(int x, int y) { YunuDX11Renderer::SingleInstance.Get().OnMouseDown(x, y); }
        virtual void OnMouseUp(int x, int y) { YunuDX11Renderer::SingleInstance.Get().OnMouseUp(x, y); }
        virtual void OnMouseMove(int btnState, int x, int y) { YunuDX11Renderer::SingleInstance.Get().OnMouseMove(btnState, x, y); }

        // 창 크기가 변했을 때 처리해야 할 것들
        virtual void OnResize() { YunuDX11Renderer::SingleInstance.Get().OnResize(); }

        // 혹여나 내부 그래픽스 구조를 알아야 할 때
        virtual ID3D11Device* QueryD3d11Device();
        //{ return YunuDX11Renderer::SingleInstance.Get().md3dDevice; };
        virtual ID3D11DeviceContext* QueryD3dDeviceContext() { return YunuDX11Renderer::SingleInstance.Get().md3dDeviceContext; }
    };
}
#endif

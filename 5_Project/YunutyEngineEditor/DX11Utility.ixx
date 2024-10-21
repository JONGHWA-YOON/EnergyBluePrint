module;
#include <d3d11.h>
#include <Windows.h>
export module utility.DX11Utility;

export namespace utility
{
    namespace dx11Utility
    {
        inline ID3D11Device* g_pd3dDevice = NULL;
        inline ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
        inline IDXGISwapChain* g_pSwapChain = NULL;
        inline ID3D11RenderTargetView* g_mainRenderTargetView = NULL;
        bool CreateDeviceD3D(HWND hWnd);
        void CleanupDeviceD3D();
        void CreateRenderTarget();
        void CleanupRenderTarget();
    }
}
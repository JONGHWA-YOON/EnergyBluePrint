#include "YunutyRenderer.h"
#include "YunutyEngine.h"
#include "IYunuGI3DRenderer.h"
#include <assert.h>

using namespace YunutyEngine::Graphics;
unique_ptr<YunutyRenderer> YunutyEngine::Graphics::YunutyRenderer::instance = nullptr;

YunutyRenderer& YunutyEngine::Graphics::YunutyRenderer::GetInstance()
{
    if (!instance.get())
        instance = make_unique<YunutyRenderer>();
    return *instance;
}
void YunutyEngine::Graphics::YunutyRenderer::Render()
{
    assert(graphicsRenderer != nullptr, L"그래픽스 렌더러가 없는 상태입니다!");
    graphicsRenderer->BeginRender();
    graphicsRenderer->Render();
    afterRenderAction();
    graphicsRenderer->EndRender();
}
void YunutyEngine::Graphics::YunutyRenderer::Update(float delta)
{
    graphicsRenderer->Update(delta);
}
void YunutyEngine::Graphics::YunutyRenderer::LoadGraphicsDll(const wstring& dllFileName)
{
    HINSTANCE hDLL{ LoadLibrary(dllFileName.c_str()) };
    assert(hDLL != nullptr, L"그래픽스 엔진에 해당하는 DLL 파일이 없습니다!");

    using FunctionType = Yunu3DAPI::IYunuGI3DRenderer* (*)();
    FunctionType CreateRenderer{ (FunctionType)GetProcAddress(hDLL, "CreateRenderer") };
    assert(CreateRenderer != nullptr, L"그래픽스 렌더러 생성 함수가 없습니다!");
    graphicsRenderer = CreateRenderer();
    assert(graphicsRenderer != nullptr, L"그래픽스 렌더러가 결국 생성되지 않았습니다!");
    graphicsRenderer->Initialize();
}
void YunutyEngine::Graphics::YunutyRenderer::SetOutputWindow(const HWND& hwnd)
{
    assert(graphicsRenderer != nullptr, L"그래픽스 렌더러가 없는 상태입니다!");
    graphicsRenderer->SetOutputWindow(hwnd);
}
void YunutyEngine::Graphics::YunutyRenderer::SetAfterRenderAction(const function<void()>& action)
{
    afterRenderAction = action;
}

ID3D11Device* YunutyEngine::Graphics::YunutyRenderer::QueryD3d11Device()
{
    return graphicsRenderer->QueryD3d11Device();
};
ID3D11DeviceContext* YunutyEngine::Graphics::YunutyRenderer::QueryD3d11DeviceContext()
{
    return graphicsRenderer->QueryD3dDeviceContext();
}
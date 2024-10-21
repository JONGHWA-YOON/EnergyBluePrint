#include <assert.h>
#include "GraphicsRenderer.h"
#include "_YunuGIObjects.h"

namespace yunutyEngine::graphics
{
    void _YunuGIObjects::LoadDll(const std::wstring& dllFileName)
    {
        // CreateResourceManager 코드 짜고 연결하면 됨
        HINSTANCE hDLL{ LoadLibraryW(dllFileName.c_str()) };
        assert(hDLL != nullptr, L"그래픽스 엔진에 해당하는 DLL 파일이 없습니다!");

        yunuGI::Global::CreateRenderer = reinterpret_cast<decltype(yunuGI::Global::CreateRenderer)>(GetProcAddress(hDLL, "CreateRenderer"));
        assert(yunuGI::Global::CreateRenderer != nullptr, L"그래픽스 렌더러 생성 함수가 없습니다!");

        yunuGI::Global::CreateFactory = reinterpret_cast<decltype(yunuGI::Global::CreateFactory)>(GetProcAddress(hDLL, "CreateFactory"));
        assert(yunuGI::Global::CreateFactory != nullptr, L"그래픽스 팩토리 생성 함수가 없습니다!");

        yunuGI::Global::CreateEventHandler = reinterpret_cast<decltype(yunuGI::Global::CreateEventHandler)>(GetProcAddress(hDLL, "CreateEventHandler"));
        assert(yunuGI::Global::CreateEventHandler != nullptr, L"그래픽스 이벤트 시스템 핸들러 생성 함수가 없습니다!");

        yunuGI::Global::CreateResourceManager = reinterpret_cast<decltype(yunuGI::Global::CreateResourceManager)>(GetProcAddress(hDLL, "CreateResourceManager"));
        assert(yunuGI::Global::CreateResourceManager != nullptr, L"그래픽스 리소스 매니저 생성 함수가 없습니다!");

        yunuGI::Global::ReleaseYunuGI = reinterpret_cast<decltype(yunuGI::Global::ReleaseYunuGI)>(GetProcAddress(hDLL, "ReleaseYunuGI"));
        assert(yunuGI::Global::ReleaseYunuGI != nullptr, L"그래픽스 인터페이스 릴리즈 함수가 없습니다!");

        renderer = yunuGI::Global::CreateRenderer({});
        assert(renderer.Get() != nullptr, L"그래픽스 렌더러가 생성되지 않았습니다!");
        //renderer->Initialize();

        eventHandler = yunuGI::Global::CreateEventHandler({});
        assert(eventHandler.Get() != nullptr, L"그래픽스 이벤트 시스템 핸들러가 생성되지 않았습니다!");

        resourceManager = yunuGI::Global::CreateResourceManager({});
        assert(resourceManager.Get() != nullptr, L"그래픽스 리소스 매니저가 생성되지 않았습니다!");

        factory = yunuGI::Global::CreateFactory(yunuGI::IFactoryDesc());
        assert(factory.Get() != nullptr, L"그래픽스 렌더러가 생성되지 않았습니다!");
    }
}

#pragma once
#include <Windows.h>
#include <vector>
#include <unordered_set>
#include <functional>
#include <memory>
#include <string>
#include "YunuGraphicsInterface.h"

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11ShaderResourceView;

namespace yunuGI
{
    class I3DRenderer;
}
namespace yunutyEngine
{
    class YunutyCycle;
    namespace graphics
    {
        class _YunuGIObjects;
        template<typename T>
        class Renderable;
    }
}
using namespace std;
namespace yunutyEngine
{
    namespace graphics
    {
        class YUNUTY_API Renderer
        {
        public:
            static Renderer& SingleInstance();
            // 그래픽스 엔진 라이브러리의 코드를 가져옵니다. 
            // 대상이 되는 그래픽 엔진은 YunuGraphicsInterface 인터페이스를 구현해야 합니다.
            void LoadGraphicsDll(const wstring& dllFileName);

            // 그래픽 결과를 출력할 창을 지정합니다. 이 창에는 게임엔진과 연계된 그래픽스 엔진만이 영향을 주어야 합니다. 
            // 같은 창에 여러 그래픽스 엔진이 렌더링을 시도할 경우, 갖은 애로사항이 발생하게 될 것입니다.
            void SetOutputWindow(const HWND& hwnd);

            // 해당 폴더와 하위 폴더 내부에 있는 모든 그래픽스 파일들을 로드합니다.
            void LoadFiles(const char* rootPath);
            // 파일을 로드합니다. 파일을 그래픽스 파일로 인지할 수 없을 경우, 해당 파일을 무시합니다.
            void LoadFile(const char* fileName);
            // 렌더링이 끝난 직후 호출할 콜백함수를 지정합니다.
            void SetAfterRenderAction(const function<void()>& action);

            // 그래픽 결과를 저장한 텍스처 객체의 핸들을 요청합니다. Null이 반환될 경우, SetOutputWindow 함수에만 의존해야 합니다.
            void* QuerySharedOutputHandle();

            // 수동으로 그래픽스 엔진의 렌더링 동작을 실행시킵니다.
            void ManualRender() { Render(); }

            // 수동으로 그래픽스 엔진의 업데이트 함수를 실행시킵니다.
            void ManualUpdate(float delta) { Update(delta); }

            
            // 그래픽스 엔진의 ClickEvent를 발생시킵니다. x,y 좌표는 0에서 1 사이의 값으로 정규화되어야 하며,
            // (0,0)의 좌표는 왼쪽 위 지점에 해당합니다.
            // 이 함수는 그래픽스 객체에 대응되는 EventHandler 객체의 이벤트 함수를 호출합니다.
            void InvokeClickEvent(float x, float y);
            void InvokeHoverEvent(float x, float y);

            // 그래픽스 엔진의 ClickEvent를 발생시킵니다. x,y 좌표는 0에서 1 사이의 값으로 정규화되어야 하며,
            // (0,0)의 좌표는 왼쪽 위 지점에 해당합니다.
            // 이 함수는 그래픽스 객체에 대응되는 Yunuty Renderable Component를 반환하며, 인게임 이벤트를 유발하진 않습니다.
            Renderable<yunuGI::IRenderable>* ClickRenderable(float x, float y);
            const yunuGI::IResourceManager* GetResourceManager();

            // 그래픽스 엔진에서 클릭이벤트가 발생했을 때, 이를 처리해줄 콜백 함수를 등록합니다.
            //void RegisterClickEventHandler(function<void(yunuGI::IRenderable*)> callback) 
            //{ clickEventCallback = callback; };
        private:
            //static void CallClickEventCallback(yunuGI::IRenderable* arg) { instance->clickEventCallback(arg); }
            void Render();
            void Update(float delta);
        private:
            static unique_ptr<Renderer> instance;
            function<void()> afterRenderAction{ []() {} };
            //function<void(yunuGI::IRenderable*)> clickEventCallback{ [](yunuGI::IRenderable*) {} };
            friend yunutyEngine::YunutyCycle;
            friend yunutyEngine::graphics::_YunuGIObjects;
        };
    }
}

#pragma once
#include <unordered_map>
#include <functional>
#include <concepts>
#include "Object.h"
#include "Component.h"
#include "Rect.h"
#include "YunuGraphicsInterface.h"
#include "Transform.h"

// 카메라는 화면에 게임상에 존재하는 모든 그래픽 요소들을 출력하는 데에 쓰이는 객체입니다. 
// 카메라의 구현은 엔진의 동작환경,혹은 카메라의 특성에 따라 달라질 수 있기 때문에,
// 화면을 그려내기 위해 쓰이는 함수 Render는 추상 메소드로 정의됩니다.
#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

namespace yunuGI
{
    class IRenderable;
}
namespace yunutyEngine::graphics
{
    class YunutyCycle;
    template <typename YunuGIType >
    class Renderable;
    class Renderer;
    class EventHandler;
    // 모체가 되는 Renderable
    // IRenderable 객체를 게임 컴포넌트와 대응시키는 맵을 정적 멤버로 들고 있다.
    template <>
    class YUNUTY_API Renderable<yunuGI::IRenderable> : public Component
    {
    public:
        virtual ~Renderable()
        {
            yunuGIWrapperMap.erase(yunuGI);
        };
        virtual void Update()override
        {
            yunuGI->SetWorldTM(GetTransform()->GetWorldTM());
        }
    protected:
        Renderable(yunuGI::IRenderable* yunuGI) : yunuGI(yunuGI)
        {
            yunuGIWrapperMap[yunuGI] = this;
        }
        virtual void OnEnable()
        {
            yunuGI->SetActive(true);
        }
        virtual void OnDisable()
        {
            yunuGI->SetActive(false);
        }
    private:
        static std::unordered_map<yunuGI::IRenderable*, Renderable<yunuGI::IRenderable>*> yunuGIWrapperMap;
        yunuGI::IRenderable* yunuGI;
        friend Renderer;
        friend EventHandler;
    };
    // 특수한 타입에 대한 Renderable
    // 그래픽스 객체를 yunuGIPtr로 들고 있으면서 래퍼 역할을 한다.
    template <typename YunuGIType>
    class YUNUTY_API Renderable : public Renderable<yunuGI::IRenderable>
    {
    private:
        yunuGI::GIPtr<YunuGIType> instancePtr;
    protected:
        // 그래픽스 객체를 받아 이벤트 시스템에 등록해야됨. 그러면 생성자를 부를때 레퍼런스가 생성되어 있어야 하는데?
        Renderable(YunuGIType* yunuGI) : Renderable<yunuGI::IRenderable>(yunuGI)
        {
            auto castedBasePtr = static_cast<yunuGI::IRenderable*>(yunuGI);
            static_assert(std::is_base_of<yunuGI::IRenderable, YunuGIType>());
            instancePtr = yunuGI;
        }
        YunuGIType& GetGI() { return *instancePtr.Get(); }
    public:
        friend Renderer;
        friend EventHandler;
    };
}

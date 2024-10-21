#pragma once
#include <thread>
#include "Object.h"
#include <functional>
#include <mutex>

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

namespace yunutyEngine
{
    class Component;
    class YUNUTY_API YunutyCycle : Object
    {
    private:
        thread updateThread;
        bool isGameRunning{ false };

        void ActiveComponentsDo(function<void(Component*)> todo);
        void ActiveComponentsDo(void (Component::* method)());
        vector<Component*> GetActiveComponents();
        vector<GameObject*> GetGameObjects(bool onlyActive = true);
        vector<std::function<void()>> afterUpdateActions;
        std::mutex actionReservationMutex;
        
        static void UpdateComponent(Component* component);
        static void StartComponent(Component* component);
        void ThreadFunction();
    protected:
        static YunutyCycle* _instance;
        YunutyCycle();
        virtual ~YunutyCycle();
        virtual void ThreadUpdate();
    public:
        // 수동으로 렌더링을 구동할 것 같으면 false로 두면 된다.
        bool autoRendering = true;
        static YunutyCycle& SingleInstance();
        virtual void Initialize();
        virtual void Release();
        void Play();
        void Stop();
        void Pause();
        // 엔진을 활용하는 스레드와 동시성 문제가 생길때 사용하는 함수
        // 업데이트가 끝난후 동작시킬 함수를 등록한다. 한번 실행된 함수는 그대로 삭제된다.
        void ReserveActionAfterUpdate(std::function<void()> action);
        void SetMaxFrameRate();
        bool IsGameRunning();
    };
}

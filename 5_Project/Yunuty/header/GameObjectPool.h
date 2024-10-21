#pragma once
#include "Component.h"
#include "GameObject.h"
#include "Scene.h"
#include <functional>
#include <unordered_set>
#include <vector>

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

using namespace std;
using namespace yunutyEngine;
namespace yunutyEngine
{
    // 오브젝트가 필요하면 생성하고, 생성된 오브젝트의 용도가 다 끝나면 폐기하는 대신 비활성화만 시켜놨다가,
    // 다시 오브젝트에 대한 요청이 들어오면 재활성화 시키는 오브젝트 풀 객체입니다. 
    template<typename ReprenstativeComponent>
    class GameObjectPool : public Component
    {
        static_assert(std::is_base_of<Component, ReprenstativeComponent>::value, "only derived classes from component are allowed");
    public:
        // 빌려줄 오브젝트가 단 하나도 없을 경우, 게임오브젝트와 그에 딸린 두목 컴포넌트를 생성합니다.
        // 그 후, 생성된 게임 오브젝트와 두목 컴포넌트를 매개변수로 initializer를 실행합니다.
        void SetObjectInitializer(function<void(GameObject*, ReprenstativeComponent*)> initializer);
        // 게임 오브젝트 풀에 저장된 게임 오브젝트를 활성화합니다.
        ReprenstativeComponent* Borrow();
        // 게임 오브젝트 풀에서 관리하는 게임 오브젝트를 되돌려 줍니다.
        void Return(ReprenstativeComponent*);
        int poolObjectsSize() { return poolObjects.size(); };
        int expendableObjectsSize() { return expendableObjects.size(); };
    protected:
        const unordered_set<ReprenstativeComponent*>& GetPoolObjects() {
            return poolObjects; 
        }
    private:
        unordered_set<ReprenstativeComponent*> poolObjects;
        unordered_set<ReprenstativeComponent*> expendableObjects;
        function<void(GameObject*, ReprenstativeComponent*)> objectInitializer = [](GameObject*, ReprenstativeComponent*) {};
    };
}
template<typename ReprenstativeComponent>
void GameObjectPool<ReprenstativeComponent>::SetObjectInitializer(function<void(GameObject*, ReprenstativeComponent*)> initializer)
{
    this->objectInitializer = initializer;
}
template<typename ReprenstativeComponent>
ReprenstativeComponent* GameObjectPool<ReprenstativeComponent>::Borrow()
{
    if (expendableObjects.empty())
    {
        auto gameObject = GetGameObject()->GetScene()->AddGameObject();
        auto component = gameObject->AddComponent<ReprenstativeComponent>();
        objectInitializer(gameObject, component);
        poolObjects.insert(component);
        expendableObjects.insert(component);
    }
    auto target = *expendableObjects.begin();
    expendableObjects.erase(target);
    target->GetGameObject()->SetSelfActive(true);
    return target;
}
template<typename ReprenstativeComponent>
void GameObjectPool<ReprenstativeComponent>::Return(ReprenstativeComponent* obj)
{
    if (poolObjects.find(obj) != poolObjects.end())
    {
        expendableObjects.insert(obj);
        obj->GetGameObject()->SetSelfActive(false);
    }
}

#pragma once
#include <iostream>
#include "Object.h"
#include "IGameObjectParent.h"
#include <unordered_map>
#include "Transform.h"
#include "Cache.h"
#include <assert.h>
#include <functional>

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

using namespace std;
namespace yunutyEngine
{
    class Scene;
    class Component;
    class Transform;
    class YunutyCycle;
    /// <summary>
    /// 게임 오브젝트는 게임 씬에 배치될 수 있는 가장 기본적인 단위의 객체입니다.
    /// 게임 오브젝트는 기본적으로 Transform 컴포넌트를 갖고 있으며, 다양한 컴포넌트를 추가로 가짐으로서 게임오브젝트로서의 성질이 결정됩니다.
    /// 게임 오브젝트는 자식 게임 오브젝트와 부모 게임 오브젝트를 가질 수 있는 계층 구조를 갖고 있습니다.
    /// 자식 게임 오브젝트의 위치와 각도는 부모 게임 오브젝트의 위치와 각도에 종속적입니다.
    /// </summary>
    class YUNUTY_API GameObject :
        public Object, public IGameObjectParent
    {
    public:
        GameObject(GameObject&) = delete;
        GameObject& operator=(GameObject&) = delete;
        GameObject() = default;
        virtual const vector<GameObject*>& GetChildren()const override;
        ~GameObject();
        Transform* GetTransform();
        const Transform* GetTransform()const;
        GameObject* AddGameObject();

        template<typename ComponentType>
        ComponentType* AddComponent()
        {
            return AddComponent<ComponentType>(GUID_NULL);
        }
        // guid를 사용하는 코드는 사실 public이면 안된다. 게임 클라이언트는 얘를 알 필요가 없고,
        // 씬의 저장, 불러오기 기능을 구현하는 게임 에디터에만 노출해야 하기 때문.
        template<typename ComponentType>
        ComponentType* AddComponent(const char* guid)
        {
            UUID uuid;
            UuidFromStringA(static_cast<RPC_CSTR>(guid), &uuid);
            return AddComponent<ComponentType>(uuid);
        }
        template<typename ComponentType>
        ComponentType* AddComponent(WCHAR* guid)
        {
            UUID uuid;
            //UuidFromStringW(reinterpret_cast<RPC_WSTR>(guid), &uuid);
            CLSIDFromString(guid, &uuid);
            return AddComponent<ComponentType>(uuid);
        }
        // 템플릿 매개변수로 주어진 타입의 컴포넌트를 게임 오브젝트에 부착합니다.
        template<typename ComponentType>
        ComponentType* AddComponent(GUID guid)
        {
            static_assert(std::is_base_of<Component, ComponentType>::value, "only derived classes from component are allowed");
            Component::addComponentDesc.gameObject = this;
            Component::addComponentDesc.guid = guid;
#if _DEBUG
            Component::addComponentDesc.addAvailable = true;
#endif
            auto newComponent = new ComponentType();
            newComponent->gameObject = this;
            components.insert(make_pair(newComponent, unique_ptr<Component>(newComponent)));
            indexedComponents.push_back(newComponent);
            return newComponent;
        }
        Component* GetComponentByIndex(unsigned int index)
        {
            // out of index 오류가 뜰 경우, 그냥 터지게 내버려 두는게 맞다.
            assert("out of index 오류가 발생했다. 컴포넌트 갯수도 파악 안하니까 오류가 나는 것 아니냐?" && index < indexedComponents.size());
            return indexedComponents[index];
        }
        // 템플릿 매개변수로 주어진 타입의 컴포넌트가 게임 오브젝트에 존재하는지 확인하고, 최초로 확인된 컴포넌트 객체의 포인터를 반환합니다.
        template<typename ComponentType>
        ComponentType* GetComponent()
        {
            //static_assert(std::is_base_of<Component, ComponentType>::value, "only derived classes from component are allowed");
            for (auto i = components.begin(); i != components.end(); i++)
            {
                auto castedPointer = dynamic_cast<ComponentType*>(i->first);
                if (castedPointer)
                    return castedPointer;
            }
            return nullptr;
        }
        // 템플릿 매개변수로 주어진 타입의 컴포넌트가 게임 오브젝트에 존재하는지 확인하고, 최초로 확인된 컴포넌트 객체의 포인터를 반환합니다.
        template<typename ComponentType>
        const ComponentType* GetComponent()const
        {
            static_assert(std::is_base_of<Component, ComponentType>::value, "only derived classes from component are allowed");
            for (auto i = components.begin(); i != components.end(); i++)
            {
                auto castedPointer = dynamic_cast<const ComponentType*>(i->first);
                if (castedPointer)
                    return castedPointer;
            }
            return nullptr;
        }
        // 템플릿 매개변수로 주어진 타입의 컴포넌트들이 게임 오브젝트에 존재하는지 확인하고, 확인된 컴포넌트들의 포인터를 벡터 컨테이너에 담아 반환합니다.
        template<typename ComponentType>
        vector<ComponentType*> GetComponents()
        {
            vector<ComponentType*> ret;
            static_assert(std::is_base_of<Component, ComponentType>::value, "only derived classes from component are allowed");
            for (auto i = components.begin(); i != components.end(); i++)
            {
                auto castedPointer = dynamic_cast<ComponentType*>(i->first);
                if (castedPointer)
                    ret.push_back(castedPointer);
            }
            return ret;
        }
        const vector<Component*>& GetIndexedComponents() { return indexedComponents; }
        void DeleteComponent(Component* component);
        // 게임 오브젝트가 활성화되어 있는지의 여부를 반환합니다.
        // 부모 객체가 활성화되어 있지 않다면 무조건 비활성화된 상태로 표시됩니다.
        bool GetActive();
        // 게임 오브젝트가 스스로 활성화되어 있는지의 여부를 반환합니다.
        // 부모 객체가 활성화되어 있지 않다면 전역적으로 활성화된 상태는 아닐수도 있습니다.
        bool GetSelfActive();
        void SetSelfActive(bool selfActive);
        // 게임 오브젝트의 부모 게임 오브젝트를 반환합니다. 부모가 없다면 null 포인터를 반환합니다.
        GameObject* GetParentGameObject();
        const GameObject* GetParentGameObject()const;
        Scene* GetScene();
        void SetParent(IGameObjectParent* parent = nullptr);
        int GetChildIndex()const;
        void SetChildIndex(int index);
        int GetSceneIndex()const;
        string getName()const;
        void setName(const string& name);
    private:
        Scene* scene = nullptr;
        string name = "";
        bool selfActive = true;
        int childrenNum = 0;
        IGameObjectParent* parent = nullptr;
        GameObject* parentGameObject = nullptr;
        Transform* transform = nullptr;
        unordered_map<GameObject*, unique_ptr<GameObject>> children;
        vector<GameObject*> childrenIndexed;
        unordered_map<const GameObject*, int> childIndexMap;
        // sceneIndex is Updated per every cycle.
        int sceneIndex = 0;
        mutable cache<int> cachedSceneIndex;
        unordered_map<Component*, unique_ptr<Component>> components;
        vector<Component*> indexedComponents;
        GameObject(IGameObjectParent* parent);
        void DoThingsOnParents(function<void(GameObject*)> todo);
        virtual void SetChildIndex(GameObject* child, int index);
        // 꼬리재귀를 위한 재귀함수
        static int GetSceneIndex(const GameObject* target);
        void SetCacheDirty();
    protected:
        unique_ptr<yunutyEngine::GameObject> MoveChild(GameObject* child) override;
        void ReceiveChild(remove_reference<unique_ptr<GameObject>>::type&& child) override;
        int GetChildIndex(const GameObject* child)const override;

        friend Scene;
        friend YunutyCycle;
#if _DEBUG
        friend Component::Component();

    private:
    public:
        static int messyIndexingCalled;
#endif
    };
}

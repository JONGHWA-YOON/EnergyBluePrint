#include "YunutyEngine.h"
#include "GameObject.h"
#include "Component.h"
#include "Transform.h"
#include <stack>

using namespace yunutyEngine;
using namespace std;

yunutyEngine::GameObject::GameObject(IGameObjectParent* parent)
{
    transform = AddComponent<Transform>();
    SetParent(parent);
}
void yunutyEngine::GameObject::DoThingsOnParents(function<void(GameObject*)> todo)
{
    auto node = this->parentGameObject;
    while (node)
    {
        todo(node);
        node = node->parentGameObject;
    }
}
void yunutyEngine::GameObject::DeleteComponent(Component* component)
{
    // 벡터에서 컴포넌트를 삭제하는데에 쓰이는 매우 추한 코드, 물론 컴포넌트 갯수가 
    // 100개를 넘어가지도 않을테니 이런 코드는 좀 비효율적이어도 된다.
    for (auto itr = indexedComponents.begin(); itr != indexedComponents.end(); itr++)
        if (*itr == component)
        {
            indexedComponents.erase(itr);
            break;
        }

    components.erase(component);
}
bool yunutyEngine::GameObject::GetActive()
{
    if (!selfActive)
        return false;
    if (parentGameObject)
        return parentGameObject->GetActive();
    else
        return true;
}
bool yunutyEngine::GameObject::GetSelfActive()
{
    return selfActive;
}
void yunutyEngine::GameObject::SetSelfActive(bool selfActive)
{
    bool activeBefore = GetActive();
    bool activeAfter;

    this->selfActive = selfActive;

    activeAfter = GetActive();
    if (activeBefore != activeAfter)
    {
        stack<GameObject*> activeStack;
        activeStack.push(this);
        while (!activeStack.empty())
        {
            auto child = activeStack.top();
            activeStack.pop();
            for (auto each : child->childrenIndexed)
                if (each->selfActive)
                    activeStack.push(each);

            for (auto eachComp = child->components.begin(); eachComp != child->components.end(); eachComp++)
                if (activeAfter)
                    eachComp->first->OnEnable();
                else
                    eachComp->first->OnDisable();
        }
    }

}
GameObject* yunutyEngine::GameObject::GetParentGameObject()
{
    return parentGameObject;
}
const GameObject* yunutyEngine::GameObject::GetParentGameObject()const
{
    return parentGameObject;
}
Scene* yunutyEngine::GameObject::GetScene()
{
    return scene;
}
void yunutyEngine::GameObject::SetParent(IGameObjectParent* parent)
{
    DoThingsOnParents([](GameObject* parent) {parent->childrenNum--; });
    if (this->parent)
        parent->ReceiveChild(this->parent->MoveChild(this));
    else
        parent->ReceiveChild(move(unique_ptr<GameObject>(this)));
    this->parent = parent;
    this->parentGameObject = dynamic_cast<GameObject*>(parent);
    DoThingsOnParents([](GameObject* parent) {parent->childrenNum++; });
}
// 이거 복잡도 n임.
unique_ptr<yunutyEngine::GameObject> yunutyEngine::GameObject::MoveChild(GameObject* child)
{
    auto ret = move(children[child]);
    auto index = GetChildIndex(child);
    children.erase(child);
    childIndexMap.erase(child);
    childrenIndexed.erase(childrenIndexed.begin() + index);
    for (int i = index; i < childrenIndexed.size(); i++)
        childIndexMap[childrenIndexed[i]] = i;

    return ret;
}

const vector<GameObject*>& yunutyEngine::GameObject::GetChildren()const
{
    return childrenIndexed;
}
void yunutyEngine::GameObject::ReceiveChild(remove_reference<unique_ptr<GameObject>>::type&& child)
{
    auto ptr = child.get();
    children.insert(make_pair(ptr, unique_ptr<GameObject>()));
    children[ptr] = move(child);
    childIndexMap.insert(make_pair(ptr, childrenIndexed.size()));
    childrenIndexed.push_back(ptr);
}
int yunutyEngine::GameObject::GetChildIndex(const GameObject* child)const
{
    auto found = childIndexMap.find(child);
    return found == childIndexMap.end() ? -1 : found->second;
}
yunutyEngine::GameObject::~GameObject()
{

}
Transform* yunutyEngine::GameObject::GetTransform()
{
    return transform;
}
const Transform* yunutyEngine::GameObject::GetTransform()const
{
    return transform;
}
GameObject* yunutyEngine::GameObject::AddGameObject()
{
    return scene->AddGameObject(this);
}
int yunutyEngine::GameObject::GetChildIndex()const
{
    return parent->GetChildIndex(this);
}
void yunutyEngine::GameObject::SetChildIndex(int index)
{
    parent->SetChildIndex(this, index);
}
void yunutyEngine::GameObject::SetChildIndex(GameObject* child, int index)
{
    if (index >= children.size())
        index = children.size() - 1;
    if (index < 0)
        index = 0;

    if (children.find(child) == children.end())
        return;
    auto origin = childIndexMap[child];
    if (origin == index)
        return;

    if (origin > index)
    {
        for (int i = origin; i > index; i--)
            childrenIndexed[i] = childrenIndexed[i - 1];
        childrenIndexed[index] = child;

        for (int i = origin; i >= index; i--)
            childIndexMap[childrenIndexed[i]] = i;
    }
    else
    {
        for (int i = origin; i < index; i++)
            childrenIndexed[i] = childrenIndexed[i + 1];
        childrenIndexed[index] = child;

        for (int i = origin; i <= index; i++)
            childIndexMap[childrenIndexed[i]] = i;
    }
}
int yunutyEngine::GameObject::GetSceneIndex()const
{
    return GetSceneIndex(this);
}
int yunutyEngine::GameObject::GetSceneIndex(const GameObject* target)
{
    if (!target->cachedSceneIndex.IsDirty())
        return target->cachedSceneIndex;

#if _DEBUG
    messyIndexingCalled++;
#endif
    stack<const GameObject*> objStack;
    objStack.push(target);
    while (!objStack.empty())
    {
        auto obj = objStack.top();
        if (!obj->cachedSceneIndex.IsDirty())
        {
            objStack.pop();
            if (!objStack.empty())
                objStack.top()->cachedSceneIndex = objStack.top()->cachedSceneIndex + obj->cachedSceneIndex;
            continue;
        }
        else
        {
            int childIndex = obj->parent->GetChildIndex(obj);
            if (childIndex == 0)
            {
                if (obj->parentGameObject)
                {
                    objStack.push(obj->parentGameObject);
                    obj->cachedSceneIndex = 1;
                    continue;
                }
                // 씬에서 가장 첫번째로 배치된 게임오브젝트
                else
                {
                    obj->cachedSceneIndex = 0;
                    continue;
                }
            }
            else
            {
                auto brother = obj->parent->GetChildren()[childIndex - 1];
                obj->cachedSceneIndex = brother->childrenNum + 1;
                objStack.push(brother);
            }
        }
    }
    return target->cachedSceneIndex;
    //int childIndex = target->parent->GetChildIndex(target);
    //if (childIndex == 0)
    //{
    //    if (target->parentGameObject)
    //        return GetSceneIndexRecursive(target->parentGameObject, deltaIndex + 1);
    //    else
    //        return deltaIndex;
    //}
    //else
    //{
    //    auto brother = target->parent->GetChildren()[childIndex - 1];
    //    ret += brother->GetSceneIndex() + brother->childrenNum;
    //}

    //return target->cachedSceneIndex = ret;
}
string yunutyEngine::GameObject::getName()const
{
    return name;
}
void yunutyEngine::GameObject::setName(const string& name)
{
    this->name = name;
}
void yunutyEngine::GameObject::SetCacheDirty()
{
    cachedSceneIndex.SetDirty();
}

#if _DEBUG
int yunutyEngine::GameObject::messyIndexingCalled = 0;
#endif

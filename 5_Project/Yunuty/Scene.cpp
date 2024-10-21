#include "YunutyEngine.h"
#include "Scene.h"
#include "GameObject.h"

using namespace yunutyEngine;
yunutyEngine::Scene* yunutyEngine::Scene::currentScene = nullptr;
yunutyEngine::Scene* yunutyEngine::Scene::getCurrentScene()
{
    return currentScene;
}
void yunutyEngine::Scene::LoadScene(Scene* scene)
{
    currentScene = scene;
}
string yunutyEngine::Scene::getName()const
{
    return name;
}
void yunutyEngine::Scene::setName(const string& name)
{
    this->name = name;
}
yunutyEngine::GameObject* yunutyEngine::Scene::AddGameObject(IGameObjectParent* parent)
{
    auto gameObject = new GameObject(parent != nullptr ? parent : this);
    gameObject->scene = this;
    /*if (!parent)
        gameObjects.insert(make_pair<gameObject, unique_ptr<GameObject>(gameObject));*/

    return gameObject;
}
yunutyEngine::GameObject* yunutyEngine::Scene::AddGameObject(string name, IGameObjectParent* parent)
{
    auto gameObject = AddGameObject(parent);
    gameObject->name = name;
    return gameObject;
}
unique_ptr<yunutyEngine::GameObject> yunutyEngine::Scene::MoveChild(GameObject* child)
{
    unique_ptr<yunutyEngine::GameObject> ret = move(children[child]);
    children.erase(child);
    auto erasedIndex = childIndexMap[child];
    childIndexMap.erase(child);
    childrenIndexed.erase(childrenIndexed.begin() + erasedIndex);
    for (auto& each : childIndexMap)
        if (each.second > erasedIndex)
            each.second--;
    return ret;
}

const vector<yunutyEngine::GameObject*>& yunutyEngine::Scene::GetChildren() const
{
    return childrenIndexed;
}
void yunutyEngine::Scene::ReceiveChild(remove_reference<unique_ptr<GameObject>>::type&& child)
{
    auto ptr = child.get();
    children.insert(make_pair(ptr, unique_ptr<GameObject>()));
    children[ptr] = move(child);
    children.emplace(ptr, move(child));
    childIndexMap.insert(make_pair(ptr, childrenIndexed.size()));
    childrenIndexed.push_back(ptr);
}
int yunutyEngine::Scene::GetChildIndex(const GameObject* child)const
{
    return childIndexMap.find(child)->second;
}
void yunutyEngine::Scene::SetChildIndex(GameObject* child, int index)
{
    if (index >= children.size())
        index = children.size() - 1;
    if (index < 0)
        index = 0;

    if (children.find(child) == children.end())
        return;
    auto origin=childIndexMap[child];
    if (origin == index)
        return;

    if (origin > index)
    {
        for (int i = origin; i > index; i--)
            childrenIndexed[i] = childrenIndexed[i-1];
        childrenIndexed[index] = child;

        for (int i = origin; i >= index; i--)
            childIndexMap[childrenIndexed[i]] = i;
    }
    else
    {
        for (int i = origin; i < index; i++)
            childrenIndexed[i] = childrenIndexed[i+1];
        childrenIndexed[index] = child;

        for (int i = origin; i <= index; i++)
            childIndexMap[childrenIndexed[i]] = i;
    }
}
void yunutyEngine::Scene::DestroyGameObject(GameObject* gameObj)
{
    if (!gameObj)
        return;
    destroyList.insert(gameObj);
}

#include "YunutyEngine.h"
#include "GlobalComponent.h"

using namespace yunutyEngine;

//std::map<GlobalComponent*, unique_ptr<GlobalComponent>> YunutyEngine::GlobalComponent::globalComponents;
std::unordered_set<GlobalComponent*> yunutyEngine::GlobalComponent::globalComponents;
GlobalComponent::GlobalComponent()
{
    globalComponents.insert(this);
    //globalComponents.insert(make_pair(this, unique_ptr<GlobalComponent>(this)));
}
GlobalComponent::~GlobalComponent()
{
}

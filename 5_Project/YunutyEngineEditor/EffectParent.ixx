module;
#include "YunutyEngine.h"
export module htkg.InGame.EffectParent;

export class EffectParent : public Component
{
public:
    static GameObject* GetEffectParent()
    {
        if (effectParent == nullptr)
            effectParent = Scene::getCurrentScene()->AddGameObject()->AddComponent<EffectParent>();
        return effectParent->GetGameObject();
    }
    virtual void Update()
    {
        GetTransform()->SetWorldPosition(Vector3d::zero);
        GetTransform()->SetWorldRotation(Quaternion());
    }
private:
    static EffectParent* effectParent;
};
EffectParent* EffectParent::effectParent{nullptr};
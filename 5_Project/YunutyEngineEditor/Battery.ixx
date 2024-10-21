module;
#include "YunutyEngine.h"
export module htkg.InGame.Battery;
import htkg.InGame.IButtonInteractable;
import htkg.ResourcesID;

export class Robot;
export class Button;
export class Battery : public yunutyEngine::Component, public IButtonInteractable
{
public:
    int remainingBattery{ 0 };
    Battery()
    {
        auto staticMeshComp = GetGameObject()->AddComponent<graphics::StaticMesh>();
        staticMeshComp->GetGI().LoadMesh(resourcesID::batteryFBX.c_str());
        staticMeshComp->GetGI().LoadDiffuseMap(resourcesID::batteryDiffuseTex.c_str());
        staticMeshComp->GetGI().SetPickingMode(false);
    }
    virtual void Interact(Robot* invoker, Button* relatedButton)override;
};
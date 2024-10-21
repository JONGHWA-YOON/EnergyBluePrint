module;
#include "YunutyEngine.h"
export module htkg.InGame.Laser;
import htkg.InGame.IButtonInteractable;
import htkg.ResourcesID;

export class Robot;
export class Button;
export struct Laser: public yunutyEngine::Component, public IButtonInteractable
{
    Laser()
    {
        auto staticMeshComp = GetGameObject()->AddComponent<graphics::StaticMesh>();
        staticMeshComp->GetGI().LoadMesh(resourcesID::laserFBX.c_str());
        staticMeshComp->GetGI().LoadDiffuseMap(resourcesID::laserDiffuseTex.c_str());
        staticMeshComp->GetGI().SetPickingMode(false);
    }
    // 레이저 그래픽만 끄고, 위험지역 판정은 그냥 Stage에서 알아서 처리하게 한다.
    virtual void Interact(Robot* invoker, Button* relatedButton) { active = false; }
    bool active;
};
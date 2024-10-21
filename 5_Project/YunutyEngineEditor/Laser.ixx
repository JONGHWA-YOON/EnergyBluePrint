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
    // ������ �׷��ȸ� ����, �������� ������ �׳� Stage���� �˾Ƽ� ó���ϰ� �Ѵ�.
    virtual void Interact(Robot* invoker, Button* relatedButton) { active = false; }
    bool active;
};
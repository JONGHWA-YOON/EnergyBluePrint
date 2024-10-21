module;
#include "YunutyEngine.h"
export module htkg.InGame.Button;
import htkg.ResourcesID;

export class IButtonInteractable;
export class Button : public Component
{
public:
    Button()
    {
        auto staticMeshComp = GetGameObject()->AddComponent<graphics::StaticMesh>();
        staticMeshComp->GetGI().LoadMesh(resourcesID::buttonFBX.c_str());
        staticMeshComp->GetGI().LoadDiffuseMap(resourcesID::buttonDiffuseTex.c_str());
        staticMeshComp->GetGI().SetPickingMode(false);
    }
    IButtonInteractable* target{ nullptr };
};
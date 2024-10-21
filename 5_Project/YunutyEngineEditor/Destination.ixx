module;
#include "YunutyEngine.h"
export module htkg.InGame.Destination;
import htkg.ResourcesID;

export class Destination : public Component
{
public:
    Destination()
    {
        auto staticMeshComp = GetGameObject()->AddComponent<graphics::StaticMesh>();
        staticMeshComp->GetGI().LoadMesh(resourcesID::destinationFBX.c_str());
        staticMeshComp->GetGI().LoadDiffuseMap(resourcesID::destinationDiffuseTex.c_str());
    }
};
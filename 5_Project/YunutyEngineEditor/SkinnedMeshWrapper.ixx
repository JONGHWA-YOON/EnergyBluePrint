module;
#include "YunutyEngine.h"
export module htkg.InGame.SkinnedMeshWrapper;

export class SkinnedMeshWrapper : public graphics::SkinnedMesh 
{
public:
    string meshFilePath;
    string diffuseFilePath;
    void ApplySettings()
    {
        GetGI().LoadMesh(meshFilePath.c_str());
        GetGI().LoadDiffuseMap(diffuseFilePath.c_str());
    }
};
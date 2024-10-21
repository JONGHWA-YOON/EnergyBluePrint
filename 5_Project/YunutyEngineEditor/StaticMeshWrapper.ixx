module;
#include "YunutyEngine.h"
export module htkg.InGame.StaticMeshWrapper;

export class StaticMeshWrapper : public graphics::StaticMesh
{
public:
    string meshFilePath;
    string diffuseFilePath;
    string normalFilePath;
    void ApplySettings()
    {
        if (meshFilePath == lastMesh &&
            diffuseFilePath == lastDiffuse &&
            normalFilePath == lastNormal)
            return;
        GetGI().LoadMesh(meshFilePath.c_str());
        GetGI().LoadDiffuseMap(diffuseFilePath.c_str());
        GetGI().LoadNormalMap(normalFilePath.c_str());
        GetGI().SetPickingMode(false);
        lastMesh = meshFilePath;
        lastDiffuse= diffuseFilePath;
		lastNormal = normalFilePath;
	}
    virtual void Update()
    {
        graphics::StaticMesh::Update();
        GetGameObject()->setName(meshFilePath);
    }
	string lastMesh;
	string lastDiffuse;
	string lastNormal;
};
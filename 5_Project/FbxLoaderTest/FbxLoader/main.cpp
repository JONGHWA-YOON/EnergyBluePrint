#include <iostream>
#include <memory>
#include "fbx sdk/include/fbxsdk.h"
#include <vector>

//#pragma comment(lib,"libfbxsdk.lib")

//class FbxManager;
namespace fbxsdk
{
    class FbxManager;
}
struct MyVertex
{
    float pos[3];
};

int main()
{
    FbxManager* manager = nullptr;

    // 매니저 만들기
    manager = FbxManager::Create();

    // IOS 세팅하기
    FbxIOSettings* pIOsettings = FbxIOSettings::Create(manager, IOSROOT);
    //pIOsettings->SetBoolProp(IMP_FBX_MATERIAL, true);
    manager->SetIOSettings(pIOsettings);

    FbxImporter* pImporter = FbxImporter::Create(manager, "");

    bool success = pImporter->Initialize("../fox.FBX", -1, manager->GetIOSettings());

    // 씬을 생성
    FbxScene* pScene = FbxScene::Create(manager, "");
    pScene->GetMemberCount<FbxAnimLayer>();

    FbxAnimLayer::ClassId;
    // 씬에 fbx파일 내용을 가져옴
    pImporter->Import(pScene);

    pImporter->Destroy();



    FbxNode* pFbxRootNode = pScene->GetRootNode();

    std::vector<MyVertex> vertextList;

    if (pFbxRootNode)
    {
        for (int i = 0; i < pFbxRootNode->GetChildCount(); i++)
        {
            FbxNode* pFbxChildNode = pFbxRootNode->GetChild(i);

            FbxNodeAttribute::EType AttributeType = pFbxChildNode->GetNodeAttribute()->GetAttributeType();

            FbxMesh* pMesh = (FbxMesh*)pFbxChildNode->GetNodeAttribute();

            FbxVector4* pVerticles = pMesh->GetControlPoints();

            for (int j = 0; j < pMesh->GetPolygonCount(); j++)
            {
                int iNumVerticles = pMesh->GetPolygonSize(j);

                for (int k = 0; k < iNumVerticles; k++)
                {
                    int icontrolPointIndex = pMesh->GetPolygonVertex(j, k);

                    MyVertex vertex;
                    vertex.pos[0] = (float)pVerticles[icontrolPointIndex].mData[0];
                    vertex.pos[1] = (float)pVerticles[icontrolPointIndex].mData[1];
                    vertex.pos[2] = (float)pVerticles[icontrolPointIndex].mData[2];
                    vertextList.push_back(vertex);
                }
            }
        }
    }
}

#pragma once
#include "IYunuGIStaticMesh.h"
#include "IYunuGISkinnedMesh.h"
#include "IYunuGIDirectionalLight.h"
#include "IYunuGICamera.h"
#include "IYunuGIDebugCube.h"
#include "IYunuGIUIImage.h"
#include "IYunuGIUIText.h"
#include "IUnknown.h"

// 팩토리 객체는 카메라, 메쉬와 같이 렌더링에 필요한 객체들을 생성한다.
namespace yunuGI
{
    struct IFactoryDesc
    {
    };
    class IFactory : public yunuGI::IUnknown
    {
    public:
        virtual yunuGI::ICamera* CreateCamera(const yunuGI::ICameraDesc& desc) = 0;
        virtual yunuGI::IStaticMesh* CreateStaticMesh(const yunuGI::IStaticMeshDesc& desc) = 0;
        virtual yunuGI::ISkinnedMesh* CreateSkinnedMesh(const yunuGI::ISkinnedMeshDesc& desc)=0;
        virtual yunuGI::IDebugCube* CreateDebugCube(const yunuGI::IDebugCubeDesc& desc) = 0;
        virtual yunuGI::IUIImage* CreateUIImage(const yunuGI::IUIImageDesc& desc) = 0;
        virtual yunuGI::IUIText* CreateUIText(const yunuGI::IUITextDesc& desc) = 0;
        virtual yunuGI::IDirectionalLight* CreateDirectionalLight(const yunuGI::IDirectionalLightDesc& desc) = 0;
    };
}

#pragma once
#include "YunuGraphicsInterface.h"
#include "YunuGICameraAdapter.h"

namespace yunuGIAdapter
{
    class FactoryAdapter : public yunuGI::IFactory
    {
    public:
        FactoryAdapter(const yunuGI::IFactoryDesc& desc) {}
        virtual yunuGI::ICamera* CreateCamera(const yunuGI::ICameraDesc& desc) { return new CameraAdapter(); }
        //{ return new CameraAdapter(); }
        virtual yunuGI::IStaticMesh* CreateStaticMesh(const yunuGI::IStaticMeshDesc& desc) { return nullptr; }
        virtual yunuGI::ISkinnedMesh* CreateSkinnedMesh(const yunuGI::ISkinnedMeshDesc& desc) { return nullptr; }
        virtual yunuGI::IDebugCube* CreateDebugCube(const yunuGI::IDebugCubeDesc& desc) { return nullptr; };
        virtual yunuGI::IUIImage* CreateUIImage(const yunuGI::IUIImageDesc& desc) { return nullptr; };
        virtual yunuGI::IUIText* CreateUIText(const yunuGI::IUITextDesc& desc) { return nullptr; };
        virtual yunuGI::IDirectionalLight* CreateDirectionalLight(const yunuGI::IDirectionalLightDesc& desc) { return nullptr; }
    };
}

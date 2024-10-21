#ifdef USE_MODULE
module;
#include "YunuGraphicsInterface.h"
import YunuGIAdapter.CameraAdapter;
export module YunuGIAdapter.FactoryAdapter;

export namespace YunuGIAdapter
{
    class FactoryAdapter : public YunuGI::IFactory
    {
    public:
        virtual YunuGI::ICamera* CreateCamera(const YunuGI::ICameraDesc& desc) { return new CameraAdapter(); }
        virtual YunuGI::IStaticMesh* CreateStaticMesh(const YunuGI::IStaticMeshDesc& desc) { return nullptr; }
        virtual YunuGI::IDirectionalLight* CreateDirectionalLight(const YunuGI::IDirectionalLightDesc& desc) { return nullptr; }
    };
}
#endif

#include "YunuGraphicsInterface.h"
#include "YunuGIAdapter.h"
//#include "YunuGIRendererAdapter.h"

extern "C"
{
    __declspec(dllexport) yunuGI::I3DRenderer* CreateRenderer(const yunuGI::I3DRendererDesc& desc)
    {
        //return nullptr;
        return new yunuGIAdapter::RendererAdapter(desc);
    }
    __declspec(dllexport) yunuGI::IFactory* CreateFactory(const yunuGI::IFactoryDesc& desc)
    {
        return nullptr;
        //return new yunuGIAdapter::FactoryAdapter(desc);
    }
    __declspec(dllexport) void ReleaseYunuGI(yunuGI::IUnknown* target)
    {
        delete target;
    }
}

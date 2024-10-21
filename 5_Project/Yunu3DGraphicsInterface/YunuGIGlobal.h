#pragma once
//#include "IYunuGI3DRenderer.h"
//#include "IUnknown.h"
#include "YunuGraphicsInterface.h"

namespace yunuGI
{
    class I3DRenderer;
    class IFactory;
    class IEventHandler;
    class IResourceManager;

    class I3DRendererDesc;
    class IFactoryDesc;
    class IEventHandlerDesc;
    class IResourceManagerDesc;

    namespace Global
    {
        inline yunuGI::I3DRenderer* (*CreateRenderer)(const I3DRendererDesc& desc);
        inline yunuGI::IFactory* (*CreateFactory)(const IFactoryDesc& desc);
        inline yunuGI::IEventHandler* (*CreateEventHandler)(const IEventHandlerDesc& desc);
        inline yunuGI::IResourceManager* (*CreateResourceManager)(const IResourceManagerDesc& desc);
        inline void (*ReleaseYunuGI)(yunuGI::IUnknown*);
    };
}

//YunuGI::I3DRenderer* (*YunuGI::Global::CreateRenderer)() = nullptr;
//void (*YunuGI::Global::ReleaseYunuGI)(YunuGI::IUnknown**) = nullptr;

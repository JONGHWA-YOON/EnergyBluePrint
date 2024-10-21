#pragma once
#include <string>
#include "SingletonClass.h"
#include "YunuGraphicsInterface.h"

using namespace std;
namespace yunutyEngine
{
    namespace graphics
    {
        class _YunuGIObjects : public SingletonClass<_YunuGIObjects>
        {
        public:
            yunuGI::GIPtr<yunuGI::I3DRenderer> renderer;
            yunuGI::GIPtr<yunuGI::IFactory> factory;
            yunuGI::GIPtr<yunuGI::IEventHandler> eventHandler;
            yunuGI::GIPtr<yunuGI::IResourceManager> resourceManager;
            void LoadDll(const std::wstring& dllFileName);
        };
    }
}

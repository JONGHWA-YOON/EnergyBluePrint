#include "Yunu3DRendererAdapter.h"

extern "C"
{
    __declspec(dllexport) Yunu3DAPI::IYunuGI3DRenderer* CreateRenderer()
    {
        return new Yunu3DRendererAdapter();
    }
}

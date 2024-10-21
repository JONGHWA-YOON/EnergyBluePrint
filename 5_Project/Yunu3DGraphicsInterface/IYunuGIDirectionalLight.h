#pragma once
#include "IUnknown.h"
#include "YunuGIMatrix4x4.h"

namespace yunuGI
{
    struct Matrix4x4;
    class IDirectionalLight : public yunuGI::IUnknown
    {
    public:
        virtual void SetWorldTM(const Matrix4x4& tm) = 0;
    };
    struct IDirectionalLightDesc
    {
    };
}

#pragma once

#include "YunuGIMatrix4x4.h"
#include "IUnknown.h"

namespace yunuGI
{
    class IRenderable : virtual public yunuGI::IUnknown
    {
    public:
        virtual void SetWorldTM(const Matrix4x4& tm) =0;
        virtual void SetActive(bool isActive) =0;

    };
}

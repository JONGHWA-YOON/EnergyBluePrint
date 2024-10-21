#pragma once
#include <array>
#include "IYunuGIDebugObject.h"
#include "YunuGIMatrix4x4.h"
#include "IUnknown.h"
#include "YunuGIColor.h"

namespace yunuGI
{
    class IDebugSphere : public yunuGI::IDebugObject
    {
    public:
        virtual void SetWorldTM(const Matrix4x4& tm) = 0;
        virtual void SetActive(bool isActive) = 0;
        virtual void SetFillModeSolid()abstract;
        virtual void SetFillModeWireframe()abstract;
        virtual void SetColor(const yunuGI::Color& color) abstract;

    };
}

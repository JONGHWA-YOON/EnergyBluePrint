#pragma once
#include <array>
#include "IYunuGIRenderable.h"
#include "YunuGIMatrix4x4.h"
#include "IUnknown.h"
#include "YunuGIColor.h"

namespace yunuGI
{
    class IDebugObject : public yunuGI::IRenderable
    {
    public:
        virtual void SetFillModeSolid()abstract;
        virtual void SetFillModeWireframe()abstract;
        virtual void SetColor(const yunuGI::Color& color) abstract;
    };
}
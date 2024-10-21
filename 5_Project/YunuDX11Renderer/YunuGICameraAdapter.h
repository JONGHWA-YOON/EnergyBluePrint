#pragma once
#include "YunuGraphicsInterface.h"
#include "PairedClass.h"
#include "Camera.h"


namespace yunuGIAdapter
{
    class CameraAdapter : public PairedClass<Camera>, public yunuGI::ICamera
    {
    public:
        template<typename... Args>
        CameraAdapter(Args... args) : PairedClass<Camera>(args...) {  };
        virtual void SetWorldTM(const yunuGI::Matrix4x4& tm) {
            auto inverse = XMMatrixInverse(nullptr, reinterpret_cast<const XMMATRIX&>(tm));
            pairedObject.SetViewMatrix(XMMatrixInverse(nullptr, reinterpret_cast<const XMMATRIX&>(tm)));
        }
        virtual void SetFOV(float fov) {}
        virtual void SetNear(float near) {};
        virtual void SetFar(float far) {};
        virtual void SetAsMain() { pairedObject.SetMain(); }
    };
}
namespace jsGI
{
    class ICamera
    {
        virtual void SetView(const yunuGI::Matrix4x4& tm) abstract;
    };
}

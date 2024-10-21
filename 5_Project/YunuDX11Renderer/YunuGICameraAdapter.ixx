#ifdef USE_MODULE
module;
#include "YunuGraphicsInterface.h"
#include "PairedClass.h"
#include "Camera.h"

export module YunuGIAdapter.CameraAdapter;

export namespace YunuGIAdapter
{
    class CameraAdapter : public PairedClass<Camera>, public YunuGI::ICamera
    {
    public:
        template<typename... Args>
        CameraAdapter(Args... args) : PairedClass<Camera>(args...) {  };
        virtual void SetWorldTM(const YunuGI::Matrix4x4& tm) { pairedObject.SetViewMatrix(XMMatrixInverse(nullptr, reinterpret_cast<const XMMATRIX&>(tm))); }
        virtual void SetFOV(float fov) {}
        virtual void SetNear(float near) {};
        virtual void SetFar(float far) {};
        virtual void SetAsMain() { pairedObject.SetMain(); }
    };
}
#endif

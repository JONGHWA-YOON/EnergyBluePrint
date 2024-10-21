#pragma once
#include "YunuGIMatrix4x4.h"
#include "IUnknown.h"

namespace yunuGI
{
    class ICamera : public IUnknown
    {
    public:
        // 카메라의 월드 위치를 전송해 view matrix를 갱신한다.
        virtual void SetWorldTM(const Matrix4x4& tm) = 0;
        // 카메라의 시야각을 갱신한다.
        virtual void SetFOV(float fov) = 0;
        // 카메라 절두체의 near 플레인까지의 거리를 정의한다.
        virtual void SetNear(float near) = 0;
        // 카메라 절두체의 far 플레인까지의 거리를 정의한다.
        virtual void SetFar(float far) = 0;
        // 카메라를 메인 카메라로 만든다.
        virtual void SetAsMain() = 0;
    };
    struct ICameraDesc
    {
    };
}

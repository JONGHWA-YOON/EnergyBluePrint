#pragma once
#include <vector>
#include <string>
#include "IUnknown.h"

// 리소스 매니저는 텍스처, 메시와 같은 그래픽스 리소스들을 명시적으로 불러오거나 할당해제하고 싶을 때 사용한다.
namespace yunuGI
{
    struct IResourceManagerDesc
    {

    };
    class IResourceManager : public IUnknown
    {
    public:
        virtual std::vector<std::string> GetMeshKeys() const abstract;
        virtual std::vector<std::string> GetSkeletonKeys()const abstract;
        virtual std::vector<std::string> GetTextureKeys()const abstract;
        virtual std::vector<std::string> AnimationKeys()const abstract;
        virtual void LoadFile(const char* filePath) = 0;
        virtual void UnloadResources() = 0;
    };
}


#include "Transform.h"

#pragma once
#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

using namespace std;
namespace YunutyEngine
{
    namespace D2D
    {
        class D2DTransformWrapper
        {
        private:
            Transform* transform;
        public:
            D2DTransformWrapper(Transform& transform);
            D2DTransformWrapper(Transform* transform);
        };
    }
}

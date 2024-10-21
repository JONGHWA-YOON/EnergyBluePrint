#pragma once
#include "Component.h"
#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

namespace yunutyEngine
{
    class YUNUTY_API Graphic : public Component
    {
    public:
        virtual ~Graphic() {}
    };
}

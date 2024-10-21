#pragma once
#include "Object.h"
#include "Vector2.h"
#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif


namespace yunutyEngine
{
    class YUNUTY_API Rect :
        public Object
    {
    public:
        Rect() :Rect(0, 0) {}
        Rect(double width, double height) :width(width), height(height) {}
        bool Contains(const Vector2d& point, const Vector2d& rectCenter=Vector2d::zero);
        double width = 0;
        double height = 0;
        Vector2d pivot = Vector2d(0.5, 0.5);
    };
}

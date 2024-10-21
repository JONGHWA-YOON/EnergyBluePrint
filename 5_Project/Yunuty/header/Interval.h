#pragma once

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

namespace yunutyEngine
{
    class YUNUTY_API Interval
    {
    public:
        static Interval FromRadius(double centre, double radius);
        Interval() = default;
        Interval(double left, double right);
        double left = 0;
        double right = 0;
        bool leftClosed = true;
        bool rightClosed = true;
        bool OverlappedWith(const Interval& other)const;
        double GetLength()const;
    };
}

#pragma once
#include "Interval.h"
#include "YunutyEngine.h"

using namespace yunutyEngine;
using namespace yunutyEngine::math;
Interval::Interval(double left, double right) :
    left(left), right(right) {}
Interval Interval::FromRadius(double centre, double radius)
{
    return Interval(centre - radius, centre + radius);
}
bool Interval::OverlappedWith(const Interval& other)const
{
    return other.right > right && other.left < right ||
        other.left < left&& other.right > left ||
        other.left > left && other.right < right || // other를 포함
        other.left<left&& other.right > right;// other에게 포함됨
}
double Interval::GetLength()const
{
    return right - left;
}

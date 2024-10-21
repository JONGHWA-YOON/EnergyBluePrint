#pragma once
//***************************************************************************************
// MathHelper.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Helper math class.
//***************************************************************************************

// based on function list made by frank luna sensei, this namespace will be updated by yunu lee
// dependent on directX tool kit.

#include <Windows.h>

// LEHIDE
//#include <xnamath.h>
#include <DirectXMath.h>
using namespace DirectX;

namespace YunuMath
{
    // Returns random float in [0, 1).
    float RandF();

    // Returns random float in [a, b).
    float RandF(float a, float b);

    template<typename JsonStructType>
    JsonStructType Min(const JsonStructType& a, const JsonStructType& b)
    {
        return a < b ? a : b;
    }

    template<typename JsonStructType>
    JsonStructType Max(const JsonStructType& a, const JsonStructType& b)
    {
        return a > b ? a : b;
    }

    // returns a when t is 0, b when t is 1
    template<typename JsonStructType>
    JsonStructType Lerp(const JsonStructType& a, const JsonStructType& b, float t)
    {
        return a + (b - a) * t;
    }

    // clamps certain value between low and max
    template<typename JsonStructType>
    JsonStructType Clamp(const JsonStructType& x, const JsonStructType& low, const JsonStructType& high)
    {
        return x < low ? low : (x > high ? high : x);
    }
    constexpr long double operator"" _degree(long double);

    // Returns the polar angle of the point (x,y) in [0, 2*PI).
    // when x = 1, y = 0. angle is 0
    // when x = 0, y = 1. angle is 0.5 * pi
    float AngleFromXY(float x, float y);

    XMMATRIX InverseTranspose(CXMMATRIX M);
    XMVECTOR RandUnitVec3();
    XMVECTOR RandHemisphereUnitVec3(XMVECTOR n);

    constexpr float Infinity = FLT_MAX;
    constexpr float Pi = 3.1415926535f;
};

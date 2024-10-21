#pragma once
#include <math.h>

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

using namespace yunutyEngine;

namespace yunutyEngine
{
    template<typename real>
    class Vector3;
    template<typename real>
    class Vector2
    {
    private:
    public:
        real x;
        real y;
        Vector2();
        Vector2(real x, real y);
        Vector2(const Vector2& rhs);
        Vector2(const Vector3<real>& rhs);
        static const Vector2 zero;
        static const Vector2 one;
        static const Vector2 right;
        static const Vector2 left;
        static const Vector2 up;
        static const Vector2 down;
        static real Dot(const Vector2& lhs, const Vector2& rhs);
        static Vector2 Project(const Vector2& vector, const Vector2& onNormal);
        static Vector2 Lerp(const Vector2& a, const Vector2& b, float t);
        static Vector2 Normalize(Vector2 v);
        static Vector2 DirectionByAngle(double radian);
        double MagnitudeSqr()const;
        double Magnitude()const;
        Vector2 Normalized() const;
        double GetAngleDegree();
        double GetAngleRadian();

        Vector2& operator=(const Vector3<real>& rhs);
        Vector2& operator=(const Vector2& rhs);
        bool operator==(const Vector2& rhs) const;
        bool operator!=(const Vector2& rhs)const;
        Vector2& operator+=(const Vector3<real>& rhs);
        Vector2 operator+(const Vector3<real>& rhs) const;
        Vector2& operator+=(const Vector2& rhs);
        Vector2 operator+(const Vector2& rhs) const;
        Vector2& operator-=(const Vector3<real>& rhs);
        Vector2 operator-(const Vector3<real>& rhs) const;
        Vector2& operator-=(const Vector2& rhs);
        Vector2 operator-(const Vector2& rhs) const;
        Vector2 operator-()const;
        Vector2& operator*=(const double& scalar);
        Vector2& operator/=(const double& scalar);
        Vector2 operator*(const double& scalar)const;
        Vector2 operator/(const double& scalar)const;
    };
    typedef Vector2<float> Vector2f;
    typedef Vector2<double> Vector2d;
    typedef Vector2<int> Vector2i;

    template<typename real>
    yunutyEngine::Vector2<real> operator*(const double& scalar, const Vector2<real>& vector)
    {
        Vector2<real> ret(vector);
        ret *= scalar;
        return ret;
    }
};
template<typename real>
yunutyEngine::Vector2<real>::Vector2<real>() :Vector2<real>(0, 0) {}
template<typename real>
yunutyEngine::Vector2<real>::Vector2<real>(real x, real y)
{
    this->x = x;
    this->y = y;
}
template<typename real>
yunutyEngine::Vector2<real>::Vector2<real>(const Vector2<real>& rhs) :Vector2<real>(rhs.x, rhs.y)
{ }
template<typename real>
yunutyEngine::Vector2<real>::Vector2<real>(const Vector3<real>& rhs) : Vector2<real>(rhs.x, rhs.y)
{ }
template<typename real>
real yunutyEngine::Vector2<real>::Dot(const Vector2<real>& lhs, const Vector2<real>& rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y;
}
template<typename real>
Vector2<real> yunutyEngine::Vector2<real>::Project(const Vector2<real>& vector, const Vector2<real>& onNormal)
{
    double num = onNormal.MagnitudeSqr();
    double num2 = yunutyEngine::Vector2<real>::Dot(vector, onNormal);
    return Vector2<real>(onNormal.x * num2 / num, onNormal.y * num2 / num);
}
template<typename real>
Vector2<real> yunutyEngine::Vector2<real>::Lerp(const Vector2<real>& a, const Vector2<real>& b, float t)
{
    return (1 - t) * a + t * b;
}
template<typename real>
Vector2<real> yunutyEngine::Vector2<real>::Normalize(Vector2<real> v)
{
    double size = v.Magnitude();
    return size != 0 ? v / size : yunutyEngine::Vector2<real>::zero;
}
template<typename real>
Vector2<real> yunutyEngine::Vector2<real>::DirectionByAngle(double radian)
{
    return Vector2<real>(cos(radian), sin(radian));
}

// 벡터를 방향벡터로 계산하고, 해당 벡터와 x축벡터와의 반시계방향 각도를 0~360도 사이로 반환한다.
template<typename real>
double yunutyEngine::Vector2<real>::GetAngleDegree()
{
    return GetAngleRadian() * 180 / 3.141592;
}
template<typename real>
double yunutyEngine::Vector2<real>::GetAngleRadian()
{
    if (x == 0)
        return y > 0 ? 3.141592 * 0.5 : 3.141592 * 1.5;

    double ret = atan2(y, x);
    if (ret > 0)
    {
    }
    else
    {
        ret += 2 * 3.141592;
    }
    return ret;
}


template<typename real>
double yunutyEngine::Vector2<real>::MagnitudeSqr()const
{
    return x * x + y * y;
}
template<typename real>
double yunutyEngine::Vector2<real>::Magnitude()const
{
    return sqrt(MagnitudeSqr());
}
template<typename real>
Vector2<real> yunutyEngine::Vector2<real>::Normalized() const
{
    return Normalize(*this);
}

template<typename real>
Vector2<real>& yunutyEngine::Vector2<real>::operator=(const Vector3<real>& rhs)
{
    x = rhs.x;
    y = rhs.y;
    return *this;
}
template<typename real>
Vector2<real>& yunutyEngine::Vector2<real>::operator=(const Vector2<real>& rhs)
{
    x = rhs.x;
    y = rhs.y;
    return *this;
}
template<typename real>
bool yunutyEngine::Vector2<real>::operator==(const Vector2<real>& rhs) const
{
    return x == rhs.x && y == rhs.y;
}
template<typename real>
bool yunutyEngine::Vector2<real>::operator!=(const Vector2<real>& rhs)const
{
    //return false;
    return !(x == rhs.x && y == rhs.y);
    //return !(*this == rhs);
}
template<typename real>
Vector2<real>& yunutyEngine::Vector2<real>::operator+=(const Vector3<real>& rhs)
{
    this->x += rhs.x;
    this->y += rhs.y;
    return *this;
}
template<typename real>
Vector2<real> yunutyEngine::Vector2<real>::operator+(const Vector3<real>& rhs) const
{
    Vector2<real> ret(*this);
    ret += rhs;
    return ret;
}
template<typename real>
Vector2<real>& yunutyEngine::Vector2<real>::operator+=(const Vector2<real>& rhs)
{
    this->x += rhs.x;
    this->y += rhs.y;
    return *this;
}
template<typename real>
Vector2<real> yunutyEngine::Vector2<real>::operator+(const Vector2<real>& rhs) const
{
    Vector2<real> ret(*this);
    ret += rhs;
    return ret;
}
template<typename real>
Vector2<real>& yunutyEngine::Vector2<real>::operator-=(const Vector3<real>& rhs)
{
    this->x -= rhs.x;
    this->y -= rhs.y;
    return *this;
}
template<typename real>
Vector2<real> yunutyEngine::Vector2<real>::operator-(const Vector3<real>& rhs) const
{
    Vector2<real> ret(*this);
    ret -= rhs;
    return ret;
}
template<typename real>
Vector2<real>& yunutyEngine::Vector2<real>::operator-=(const Vector2<real>& rhs)
{
    this->x -= rhs.x;
    this->y -= rhs.y;
    return *this;
}
template<typename real>
Vector2<real> yunutyEngine::Vector2<real>::operator-(const Vector2<real>& rhs) const
{
    Vector2<real> ret(*this);
    ret -= rhs;
    return ret;
}
template<typename real>
Vector2<real> yunutyEngine::Vector2<real>::operator-()const
{
    return *this * -1;
}
template<typename real>
Vector2<real>& yunutyEngine::Vector2<real>::operator*=(const double& scalar)
{
    this->x *= scalar;
    this->y *= scalar;
    return *this;
}
template<typename real>
Vector2<real>& yunutyEngine::Vector2<real>::operator/=(const double& scalar)
{
    this->x /= scalar;
    this->y /= scalar;
    return *this;
}
template<typename real>
Vector2<real> yunutyEngine::Vector2<real>::operator*(const double& scalar)const
{
    Vector2<real> ret(*this);
    ret *= scalar;
    return ret;
}
template<typename real>
Vector2<real> yunutyEngine::Vector2<real>::operator/(const double& scalar)const
{
    Vector2<real> ret(*this);
    ret /= scalar;
    return ret;
}

template<typename real>
const Vector2<real> yunutyEngine::Vector2<real>::one = Vector2<real>(1, 1);
template<typename real>
const Vector2<real> yunutyEngine::Vector2<real>::zero = Vector2<real>(0, 0);
template<typename real>
const Vector2<real> yunutyEngine::Vector2<real>::right = Vector2<real>(1, 0);
template<typename real>
const Vector2<real> yunutyEngine::Vector2<real>::left = Vector2<real>(-1, 0);
template<typename real>
const Vector2<real> yunutyEngine::Vector2<real>::up = Vector2<real>(0, 1);
template<typename real>
const Vector2<real> yunutyEngine::Vector2<real>::down = Vector2<real>(0, -1);

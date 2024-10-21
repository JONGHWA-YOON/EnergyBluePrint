#include "YunutyEngine.h"
#include "Quaternion.h"
#include <math.h>
#include <cmath>
#include <DirectXMath.h>
#include "_XMMath.h"
using namespace yunutyEngine;

Quaternion::Quaternion() :Quaternion(1, 0, 0, 0)
{ }
Quaternion::Quaternion(double w, double x, double y, double z)
{
    this->w = w;
    this->x = x;
    this->y = y;
    this->z = z;
}
Quaternion::Quaternion(const Vector3d& euler)
{
    *this = euler;
}
Quaternion& Quaternion::operator=(const Vector3d& euler)
{
    ;
    double cr = cos(euler.x * math::Deg2Rad * 0.5);
    double sr = sin(euler.x * math::Deg2Rad * 0.5);
    double cp = cos(euler.y * math::Deg2Rad * 0.5);
    double sp = sin(euler.y * math::Deg2Rad * 0.5);
    double cy = cos(euler.z * math::Deg2Rad * 0.5);
    double sy = sin(euler.z * math::Deg2Rad * 0.5);
    w = cr * cp * cy + sr * sp * sy;
    x = sr * cp * cy - cr * sp * sy;
    y = cr * sp * cy + sr * cp * sy;
    z = cr * cp * sy - sr * sp * cy;

    return *this;
}
Vector3d Quaternion::Euler() const
{
    Vector3d angles;

    // x (x-axis rotation)
    double sinr_cosp = 2 * (w * x + y * z);
    double cosr_cosp = 1 - 2 * (x * x + y * y);
    angles.x = std::atan2(sinr_cosp, cosr_cosp);

    // y (y-axis rotation)
    double sinp = 2 * (w * y - z * x);
    if (std::abs(sinp) >= 1)
        angles.y = std::copysign(math::PI / 2, sinp); // use 90 degrees if out of range
    else
        angles.y = std::asin(sinp);

    // z (z-axis rotation)
    double siny_cosp = 2 * (w * z + x * y);
    double cosy_cosp = 1 - 2 * (y * y + z * z);
    angles.z = std::atan2(siny_cosp, cosy_cosp);

    return angles * math::Rad2Deg;
}
Quaternion Quaternion::operator*(const Quaternion& other)const
{
    Quaternion ret;
    ret.w = w * other.w - x * other.x - y * other.y - z * other.z;
    ret.x = y * other.z - z * other.y + w * other.x + x * other.w;
    ret.y = z * other.x - x * other.z + w * other.y + y * other.w;
    ret.z = x * other.y - y * other.x + w * other.z + z * other.w;
    return ret;
}
Quaternion Quaternion::inverse()const
{
    return Quaternion(w, -x, -y, -z);
}
Vector3d Quaternion::operator*(const Vector3d& other)const
{
    auto ret = (*this) * Quaternion(0, other.x, other.y, other.z) * this->inverse();
    return Vector3d(ret.x, ret.y, ret.z);
}
Vector3d Quaternion::Forward()const
{
    return *this * Vector3d::forward;
}
Vector3d Quaternion::Up()const
{
    return *this * Vector3d::up;
}
Vector3d Quaternion::Right()const
{
    return *this * Vector3d::right;
}
Quaternion Quaternion::MakeWithForwardUp(const Vector3d& forward, const Vector3d& up)
{
    Vector3d newUp = up - Vector3d::Project(up, forward.Normalized());
    Vector3d newRight = Vector3d::Cross(up, forward);
    return Quaternion::MakeWithAxes(newRight, newUp, forward);
}
Quaternion Quaternion::MakeWithAxes(const Vector3d& right, const Vector3d& up, const Vector3d& forward)
{
    return MakeWithAxes(
        Vector3f(right.x, right.y, right.z),
        Vector3f(up.x, up.y, up.z),
        Vector3f(forward.x, forward.y, forward.z)
    );
}
Quaternion Quaternion::MakeWithAxes(const Vector3f& right, const Vector3f& up, const Vector3f& forward)
{
    Vector3f&& rightN = right.Normalized();
    Vector3f&& upN = up.Normalized();
    Vector3f&& forwardN = forward.Normalized();
    auto quat = DirectX::XMQuaternionRotationMatrix(
        math::TO_XMMATRIX(yunuGI::Matrix4x4 {
        .m11 = rightN.x, .m12 = rightN.y, .m13 = rightN.z, .m14 = 0,
            .m21 = upN.x, .m22 = upN.y, .m23 = upN.z, .m24 = 0,
            .m31 = forwardN.x, .m32 = forwardN.y, .m33 = forwardN.z, .m34 = 0,
            .m41 = 0, .m42 = 0, .m43 = 0, .m44 = 1
    }));
    return Quaternion(
        static_cast<double>(quat.m128_f32[3]),
        static_cast<double>(quat.m128_f32[0]),
        static_cast<double>(quat.m128_f32[1]),
        static_cast<double>(quat.m128_f32[2])
    );
}

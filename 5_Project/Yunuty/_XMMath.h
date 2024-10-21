#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXMathMatrix.inl>
#include <assert.h>
#include <concepts>
#include "YunuGIMatrix4x4.h"
#include "Vector3.h"
#include "Quaternion.h"


namespace yunutyEngine
{
    namespace math
    {
        inline const DirectX::XMMATRIX& TO_XMMATRIX(const yunuGI::Matrix4x4& matrix)
        {
            static_assert(sizeof(yunuGI::Matrix4x4) == sizeof(DirectX::XMMATRIX));
            return reinterpret_cast<const DirectX::XMMATRIX&>(matrix);
        }
        inline DirectX::XMVECTOR TO_XMVECTOR(const Vector3f& vector, float fourthMember)
        {
            return DirectX::XMVECTOR{
               static_cast<float>(vector.x),
               static_cast<float>(vector.y),
               static_cast<float>(vector.z),
               static_cast<float>(fourthMember)
            };
        }
        inline DirectX::XMMATRIX GetTransformMatrix(const Vector3d& position, const Vector3d& scale, const Quaternion& rotation)
        {
            return DirectX::XMMatrixTransformation(
                { 0,0,0,0 },
                { 0,0,0,1 },
                { static_cast<float>(scale.x),static_cast<float>(scale.y),static_cast<float>(scale.z),0 },
                { 0,0,0,0 },
                { static_cast<float>(rotation.x),static_cast<float>(rotation.y),static_cast<float>(rotation.z),static_cast<float>(rotation.w) },
                { static_cast<float>(position.x),static_cast<float>(position.y),static_cast<float>(position.z),0 }
            );
        }
    }
}

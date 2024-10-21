
#pragma once
#include <thread>
#include "Object.h"
#include <functional>

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

using namespace std;

namespace yunutyEngine
{
    class Collider2D;
    class RigidBody2D;
    class YUNUTY_API Collision2D
    {
    public:
        Collider2D* m_Collider;
        Collider2D* m_OtherCollider;
        RigidBody2D* m_Rigidbody;
        RigidBody2D* m_OtherRigidbody;
        Collision2D() = default;
    };
}

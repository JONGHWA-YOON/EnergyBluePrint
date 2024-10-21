#include "YunutyEngine.h"
#include "RigidBody2D.h"

using namespace yunutyEngine;

void RigidBody2D::Update()
{
    if (isVelocityCached)
    {
        isVelocityCached = false;
        velocity = cachedVelocity;
    }
    GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition() + velocity * Time::GetDeltaTime());
    GetTransform()->SetWorldRotation(GetTransform()->GetWorldRotation().Euler() + angularVelocity * Time::GetDeltaTime() * Vector3d::forward);
}
void RigidBody2D::OnCollisionEnter2D(const Collision2D& collision)
{
    if (collision.m_OtherRigidbody == nullptr)
        return;

    double m1, m2;
    Vector2d u1, u2;
    m1 = mass;
    u1 = velocity;
    m2 = collision.m_OtherRigidbody->mass;
    u2 = collision.m_OtherRigidbody->velocity;

    cachedVelocity = ((u1 * (m1 - m2)) + ((2 * m2) * u2)) / (m1 + m2);
    isVelocityCached = true;
}

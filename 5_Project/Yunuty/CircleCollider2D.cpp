#include "YunutyEngine.h"

using namespace yunutyEngine;

double CircleCollider2D::GetArea()const
{
    return radius * radius * math::PI;
}
bool CircleCollider2D::isOverlappingWith(const Collider2D* other)const
{
    return other->isOverlappingWith(this);
}
bool CircleCollider2D::isOverlappingWith(const BoxCollider2D* other)const
{
    return Collider2D::isOverlapping(other, this);
}
bool CircleCollider2D::isOverlappingWith(const CircleCollider2D* other)const
{
    return Collider2D::isOverlapping(other, this);
}
bool CircleCollider2D::isOverlappingWith(const LineCollider2D* other)const
{
    return Collider2D::isOverlapping(other, this);
}
bool CircleCollider2D::isInsideNode(const QuadTreeNode* node)const
{
    auto position = GetTransform()->GetWorldPosition();
    return node->xInterval.left - radius < position.x&& position.x < node->xInterval.right + radius &&
        node->yInterval.left - radius < position.y&& position.y < node->yInterval.right + radius;
}
void CircleCollider2D::SetRadius(double radius)
{
    this->radius = radius;
}
Interval CircleCollider2D::projectedInterval(const Vector2d& v)const
{
    auto projectedCenter = Vector2d::Dot(GetTransform()->GetWorldPosition(), v);
    return Interval::FromRadius(projectedCenter, radius * GetTransform()->GetWorldScale().x);
}
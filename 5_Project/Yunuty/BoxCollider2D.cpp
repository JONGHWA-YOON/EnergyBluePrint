#include "YunutyEngine.h"
using namespace math;

bool BoxCollider2D::isOverlappingWith(const Collider2D* other)const
{
    return other->isOverlappingWith(this);
}
bool BoxCollider2D::isOverlappingWith(const BoxCollider2D* other)const
{
    return Collider2D::isOverlapping(this, other);
}
bool BoxCollider2D::isOverlappingWith(const CircleCollider2D* other)const
{
    return Collider2D::isOverlapping(this, other);
}
bool BoxCollider2D::isOverlappingWith(const LineCollider2D* other)const
{
    return Collider2D::isOverlapping(this, other);
}
bool BoxCollider2D::isInsideNode(const QuadTreeNode* node)const
{
    auto position = GetTransform()->GetWorldPosition();
    auto scale = GetTransform()->GetWorldScale();
    auto radius = (collisonRect.height + collisonRect.width)*max(scale.x,scale.y) / 2;
    return node->xInterval.left - radius < position.x&& position.x < node->xInterval.right + radius &&
        node->yInterval.left - radius < position.y&& position.y < node->yInterval.right + radius;
}
double BoxCollider2D::GetArea()const
{
    return collisonRect.height * collisonRect.width;
}
void BoxCollider2D::SetWidth(double width)
{
    collisonRect.width = width;
}
void BoxCollider2D::SetHeight(double height)
{
    collisonRect.height = height;
}
double BoxCollider2D::GetWidth()const
{
    return collisonRect.width;
}
double BoxCollider2D::GetHeight()const
{
    return collisonRect.height;
}
Interval BoxCollider2D::projectedInterval(const Vector2d& vParam)const
{
    Vector2d v = vParam.Normalized();
    auto scale = GetTransform()->GetWorldScale();
    auto scaledHeight = collisonRect.height * scale.y;
    auto scaledWidth = collisonRect.width * scale.x;
    auto angle = GetTransform()->GetWorldRotation().Euler().z * math::Deg2Rad;
    auto innerAngle = atan(scaledHeight / scaledWidth);
    auto r = Vector2d(scaledHeight, scaledWidth).Magnitude() / 2.0;

    auto projectedCenter = Vector2d::Dot(GetTransform()->GetWorldPosition(), v);
    auto projectedR = max(
        abs(Vector2d::Dot(Vector2d::DirectionByAngle(angle + innerAngle) * r, v)),
        abs(Vector2d::Dot(Vector2d::DirectionByAngle(angle - innerAngle) * r, v))
    );
    return Interval::FromRadius(projectedCenter, projectedR);
}

#include "YunutyEngine.h"
#include "LineCollider2D.h"
using namespace yunutyEngine;

bool LineCollider2D::isOverlappingWith(const BoxCollider2D* other)const
{
    return Collider2D::isOverlapping(other, this);
}
bool LineCollider2D::isOverlappingWith(const CircleCollider2D* other)const
{
    return Collider2D::isOverlapping(other, this);
}
bool LineCollider2D::isOverlappingWith(const LineCollider2D* other)const
{
    return Collider2D::isOverlapping(other, this);
}

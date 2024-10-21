#include "YunutyEngine.h"
#include "Rect.h"

bool Rect::Contains(const Vector2d& point, const Vector2d& rectCenter)
{
    return point.x <= rectCenter.x + 0.5 * width && point.x >= rectCenter.x - 0.5 * width &&
        point.y <= rectCenter.y + 0.5 * height && point.y >= rectCenter.y - 0.5 * height;
}

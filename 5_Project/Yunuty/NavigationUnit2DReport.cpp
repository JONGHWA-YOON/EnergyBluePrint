#include "NavigationUnit2DReport.h"
#include "YunutyEngine.h"

NavigationUnit2DReport::NavigationUnit2DReport(const NavigationUnit2D* unit)
    :state(unit->state), position(unit->GetTransform()->GetWorldPosition()), width(width), height(height)
{
}

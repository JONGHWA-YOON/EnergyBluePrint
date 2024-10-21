#pragma once
#include "NavigationUnit2DState.h"
#include "Vector2.h"
namespace yunutyEngine
{
    class NavigationUnit2D;
    class NavigationUnit2DReport
    {
    public:
        NavigationUnit2DReport() = default;
        NavigationUnit2DReport(const NavigationUnit2D* unit);
        Vector2d position = Vector2d::zero;
        NavigationUnit2DState state = NavigationUnit2DState::NullState;
        int width = 0;
        int height = 0;
    };

}

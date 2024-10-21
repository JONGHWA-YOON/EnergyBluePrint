#pragma once
#include "Component.h"
#include "NavigationUnit2DReport.h"

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

namespace yunutyEngine
{
    class NavigationField2D;
    class YUNUTY_API NavigationUnit2D : public Component
    {
    public:
        void ReportStatus();
    protected:
        NavigationUnit2DState state = NavigationUnit2DState::Occupying;
        int width = 1;
        int height = 1;
    private:
        NavigationUnit2DReport lastReport;
        friend NavigationUnit2DReport;
    };
}

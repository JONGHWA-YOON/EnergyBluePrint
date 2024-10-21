#include "YunutyEngine.h"

using namespace yunutyEngine;

// 자신이 속한 네비게이션 필드에 네비게이션 유닛의 
void NavigationUnit2D::ReportStatus()
{
    auto navField = NavigationField2D::GetAssignedField(GetTransform()->GetWorldPosition());
    if (navField)
    {
        auto currentReport = NavigationUnit2DReport(this);
        navField->ReceiveUnitStatus(currentReport, lastReport);
        lastReport = currentReport;
    }
}

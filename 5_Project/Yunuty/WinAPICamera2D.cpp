#include "YunutyEngine.h"
#include "WinAPICamera2D.h"
#include "WinAPIGraphic.h"
#include <algorithm>
#include "GameObject.h"
#include "WinAPICycle.h"


using namespace YunutyEngine;
using namespace YunutyEngine::WinAPI;

void WinAPICamera2D::InitHDC(HWND hWnd)
{
    hdc = GetDC(hWnd);
    hBit = CreateCompatibleBitmap(hdc, resolutionW, resolutionH);
    memDc = CreateCompatibleDC(hdc);

    HBITMAP hOldBit = (HBITMAP)SelectObject(memDc, hBit);
    DeleteObject(hOldBit);
}
void WinAPICamera2D::Render()
{
}
LRESULT CALLBACK WinAPICamera2D::Render(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT paintStruct;
    InvalidateRect(hWnd, NULL, FALSE);
    if (hdc == 0)
        InitHDC(hWnd);

    BeginPaint(hWnd, &paintStruct);

    Rectangle(memDc, -1, -1, resolutionW + 1, resolutionH + 1);

    vector<WinAPIGraphic*> graphics;
    for (auto each : WinAPIGraphic::GetWinAPIGraphics())
    {
        if (each->GetGameObject()->GetActive() && each->GetGameObject()->GetScene() == GetGameObject()->GetScene())
            graphics.push_back(each);
    }
    sort(graphics.begin(), graphics.end(), [](WinAPIGraphic*& item1, WinAPIGraphic*& item2)->bool
        {
            return item1->GetGameObject()->GetSceneIndex() > item2->GetGameObject()->GetSceneIndex();
        });

    //SelectObject(hdc, (HBRUSH)GetStockObject(BLACK_BRUSH));
    //Ellipse(hdc, -400, 400, 400, -400);
    for (auto each : graphics)
    {
        auto pos = each->GetTransform()->GetWorldPosition();
        Vector2d screenPos(resolutionW * 0.5, resolutionH * 0.5);
        screenPos += pos - GetTransform()->GetTransform()->GetWorldPosition();
        each->Render(memDc, screenPos, resolutionFactor);
    }
    BitBlt(hdc, 0, 0, resolutionW, resolutionH, memDc, 0, 0, SRCCOPY);
    EndPaint(hWnd, &paintStruct);
    return 0;
}

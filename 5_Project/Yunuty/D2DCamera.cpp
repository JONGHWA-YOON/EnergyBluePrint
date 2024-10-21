#include "YunuD2DGraphicCore.h"
#include "YunutyEngine.h"

#include "D2DCamera.h"
#include "D2DGraphic.h"
#include <algorithm>
#include "GameObject.h"
#include "D2DCycle.h"

using namespace YunutyEngine;
using namespace YunutyEngine::D2D;

void D2DCamera::Render()
{
}
LRESULT CALLBACK D2DCamera::Render(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    YunuD2D::YunuD2DGraphicCore::GetInstance()->BeginDraw();

    YunuD2D::YunuD2DGraphicCore::GetInstance()->ResizeResolution(resolutionW, resolutionH);
    SetNearPlane(Rect(resolutionW, resolutionH));

    auto halvedRenderSize = YunuD2D::YunuD2DGraphicCore::GetInstance()->GetRenderSize();
    halvedRenderSize.width /= 2;
    halvedRenderSize.height /= 2;

    vector<D2DGraphic*> graphics;

    // render world space graphics
    for (auto each : D2DGraphic::D2DGraphics[(int)CanvasRenderSpace::WorldSpace])
    {
        if (each->GetGameObject()->GetActive() && each->GetGameObject()->GetScene() == GetGameObject()->GetScene())
            graphics.push_back(each);
    }

#if _DEBUG
    GameObject::messyIndexingCalled = 0;
#endif
    sort(graphics.begin(), graphics.end(), [](const D2DGraphic* item1, const D2DGraphic* item2)->bool
        {
            return item1->GetGameObject()->GetSceneIndex() < item2->GetGameObject()->GetSceneIndex();
        });

    D2D1::Matrix3x2F eachTransform;
    Vector3d camPos;
    Vector3d pos;
    Vector3d scale;
    for (auto each : graphics)
    {
        eachTransform = D2D1::Matrix3x2F::Identity();
        camPos = GetTransform()->GetWorldPosition();
        pos = each->GetTransform()->GetWorldPosition();
        scale = each->GetTransform()->GetWorldScale();
        eachTransform = eachTransform * ScaleTransform(scale.x * 1 / zoomOutFactor, scale.y * 1 / zoomOutFactor);
        eachTransform = eachTransform * RotationTransform(each->GetTransform()->GetWorldRotation().Euler().z);
        eachTransform = eachTransform * TranslationTransform(halvedRenderSize.width + (pos.x - camPos.x) * 1 / zoomOutFactor, halvedRenderSize.height - (pos.y - camPos.y) * 1 / zoomOutFactor);

        each->Render(eachTransform);
    }
    graphics.clear();

    // render camera space graphics
    for (auto each : D2DGraphic::D2DGraphics[(int)CanvasRenderSpace::CameraSpace])
    {
        if (each->GetGameObject()->GetActive() && each->GetGameObject()->GetScene() == GetGameObject()->GetScene())
            graphics.push_back(each);
    }
    sort(graphics.begin(), graphics.end(), [](D2DGraphic*& item1, D2DGraphic*& item2)->bool
        {
            return item1->GetGameObject()->GetSceneIndex() < item2->GetGameObject()->GetSceneIndex();
        });

    for (auto each : graphics)
    {
        eachTransform = D2D1::Matrix3x2F::Identity();
        camPos = Vector2d::zero;
        pos = each->GetTransform()->GetWorldPosition();
        scale = each->GetTransform()->GetWorldScale();
        eachTransform = eachTransform * ScaleTransform(float(scale.x / zoomOutFactor), float(scale.y / zoomOutFactor));
        eachTransform = eachTransform * RotationTransform(float(each->GetTransform()->GetWorldRotation().Euler().z));
        eachTransform = eachTransform * TranslationTransform((halvedRenderSize.width + pos.x - camPos.x) * 1 / zoomOutFactor, (halvedRenderSize.height - (pos.y - camPos.y)) * 1 / zoomOutFactor);

        each->Render(eachTransform);
    }

    YunuD2D::YunuD2DGraphicCore::GetInstance()->EndDraw();
    return 0;
}

D2D1::Matrix3x2F D2DCamera::ScaleTransform(float x, float y)
{
    D2D1::Matrix3x2F ret = D2D1::Matrix3x2F::Identity();
    ret.m11 = x;
    ret.m22 = y;
    return ret;
}
D2D1::Matrix3x2F D2DCamera::RotationTransform(float angle)
{
    D2D1::Matrix3x2F ret = D2D1::Matrix3x2F::Identity();
    ret.m11 = (FLOAT)cos(angle * YunutyMath::Deg2Rad);
    ret.m12 = (FLOAT)-sin(angle * YunutyMath::Deg2Rad);
    ret.m21 = (FLOAT)sin(angle * YunutyMath::Deg2Rad);
    ret.m22 = (FLOAT)cos(angle * YunutyMath::Deg2Rad);
    return ret;
}
D2D1::Matrix3x2F D2DCamera::TranslationTransform(float dx, float dy)
{
    D2D1::Matrix3x2F ret = D2D1::Matrix3x2F::Identity();
    ret.dx = dx;
    ret.dy = dy;
    return ret;
}
void D2DCamera::SetZoomOutFactor(double zoomOutFactor)
{
    if (zoomOutFactor > 0)
        this->zoomOutFactor = zoomOutFactor;
    else
        this->zoomOutFactor = 0;
}
D2DCamera* D2DCamera::GetMainD2DCamera()
{
    return dynamic_cast<D2DCamera*>(GetMainCamera());
}


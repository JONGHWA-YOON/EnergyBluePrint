#include "YunutyEngine.h"
#include "WinAPICircle.h"
#include <ObjIdl.h>
#include <gdiplus.h>
#include "Transform.h"
#include "YunutyTime.h"
#pragma comment (lib,"Gdiplus.lib")

using namespace YunutyEngine;
using namespace YunutyEngine::WinAPI;
using namespace Gdiplus;

void WinAPICircle::Update()
{
    //GetTransform()->position.x += -10 * Time::GetDeltaTime();
}
void WinAPICircle::Render(HDC hdc, const Vector2d position, const Vector2d& scale, double angle)
{
    Pen pen(Color(255, 255, 0, 122));
    Ellipse(hdc, position.x - 400, position.y + 400, position.x + 400, position.y - 400);
}

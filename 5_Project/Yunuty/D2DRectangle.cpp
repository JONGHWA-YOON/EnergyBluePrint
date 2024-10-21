#include "YunuD2DGraphicCore.h"
#include "YunutyEngine.h"
#include "D2DRectangle.h"

using namespace YunutyEngine;
using namespace YunutyEngine::D2D;

void D2DRectangle::Render(D2D1::Matrix3x2F transform)
{
    YunuD2D::YunuD2DGraphicCore::GetInstance()->DrawRect(width,height,border,transform,color,filled);
}

#include "YunuD2DGraphicCore.h"
#include "YunutyEngine.h"
#include "D2DText.h"

using namespace YunutyEngine;
using namespace YunutyEngine::D2D;

void D2DText::Render(D2D1::Matrix3x2F transform)
{
    YunuD2D::YunuD2DGraphicCore::GetInstance()->DrawTextImage(text, transform, color,fontSize,width,height);
}

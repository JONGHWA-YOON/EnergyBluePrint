#include "YunuD2DGraphicCore.h"
#include "YunutyEngine.h"
#include "D2DGraphic.h"

using namespace YunutyEngine;
using namespace YunutyEngine::D2D;

//ID2D1Factory* YunutyEngine::D2D::D2DGraphic::direct2DFactory = nullptr;
ID2D1HwndRenderTarget* YunutyEngine::D2D::D2DGraphic::HwndRenderTarget = nullptr;
unordered_set<D2DGraphic*> YunutyEngine::D2D::D2DGraphic::D2DGraphics[(int)(CanvasRenderSpace::WorldSpace) + 1];


using namespace YunutyEngine;
D2DGraphic::D2DGraphic()
{
    SetRenderSpace(CanvasRenderSpace::WorldSpace);
}
D2DGraphic::~D2DGraphic()
{
    D2DGraphics[(int)renderSpace].erase(this);
}
void YunutyEngine::D2D::D2DGraphic::SetRenderSpace(CanvasRenderSpace renderSpace)
{
    D2DGraphics[(int)this->renderSpace].erase(this);
    this->renderSpace = renderSpace;
    D2DGraphics[(int)renderSpace].insert(this);
}
YunutyPixelInfos YunutyEngine::D2D::GetPixelInfos(wstring imgFilepath)
{
    return YunutyPixelInfos(YunuD2D::YunuD2DGraphicCore::GetInstance()->GetPixelInfos(imgFilepath));
}

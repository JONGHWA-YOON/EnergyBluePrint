#include "YunutyEngine.h"
#include "WinAPIGraphic.h"

using namespace YunutyEngine;
using namespace YunutyEngine::WinAPI;

unordered_set<WinAPIGraphic*> YunutyEngine::WinAPI::WinAPIGraphic::D2DGraphics;


WinAPIGraphic::WinAPIGraphic()
{
    D2DGraphics.insert(this);
}
WinAPIGraphic::~WinAPIGraphic()
{
    D2DGraphics.erase(this);
}
const unordered_set<WinAPIGraphic*>& YunutyEngine::WinAPI::WinAPIGraphic::GetWinAPIGraphics()
{
    return D2DGraphics;
}

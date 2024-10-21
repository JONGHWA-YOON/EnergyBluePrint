#include "DebugCube.h"
#include "YunutyEngine.h"
#include "_YunuGIObjects.h"
using namespace yunutyEngine::graphics;

yunutyEngine::graphics::DebugCube::DebugCube() :
    Renderable<yunuGI::IDebugCube>(_YunuGIObjects::SingleInstance().factory->CreateDebugCube({}))
{
}

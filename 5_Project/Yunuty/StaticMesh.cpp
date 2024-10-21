#include "YunutyEngine.h"
#include "StaticMesh.h"
#include "_YunuGIObjects.h"
using namespace yunutyEngine::graphics;

yunutyEngine::graphics::StaticMesh::StaticMesh() :
    Renderable<yunuGI::IStaticMesh>(_YunuGIObjects::SingleInstance().factory->CreateStaticMesh({}))
{
}

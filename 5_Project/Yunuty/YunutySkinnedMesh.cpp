#include "YunutyEngine.h"
#include "YunutySkinnedMesh.h"
#include "_YunuGIObjects.h"

using namespace yunutyEngine::graphics;

yunutyEngine::graphics::SkinnedMesh::SkinnedMesh() :
    Renderable<yunuGI::ISkinnedMesh>(_YunuGIObjects::SingleInstance().factory->CreateSkinnedMesh({}))
{
}

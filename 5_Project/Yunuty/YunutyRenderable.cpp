#include "YunutyRenderable.h"

std::unordered_map<yunuGI::IRenderable*, yunutyEngine::graphics::Renderable<yunuGI::IRenderable>*>
yunutyEngine::graphics::Renderable<yunuGI::IRenderable>::yunuGIWrapperMap;

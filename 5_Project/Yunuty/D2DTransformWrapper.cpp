#include "YunuD2DGraphicCore.h"
#include "YunutyEngine.h"
#include "D2DTransformWrapper.h"

YunutyEngine::D2D::D2DTransformWrapper::D2DTransformWrapper(Transform& transform)
    :transform(&transform)
{
}
YunutyEngine::D2D::D2DTransformWrapper::D2DTransformWrapper(Transform* transform)
    :transform(transform)
{
}

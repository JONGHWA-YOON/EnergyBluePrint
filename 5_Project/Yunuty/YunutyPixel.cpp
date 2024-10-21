#include "YunuD2DGraphicCore.h"
#include "pch.h"
#include "YunutyPixel.h"

using namespace YunuD2D;
using namespace YunutyEngine::D2D;

YunutyPixel::YunutyPixel(const YunuD2D::Pixel& pixel) :
    r(pixel.r),
    g(pixel.g),
    b(pixel.b),
    a(pixel.a)
{
}
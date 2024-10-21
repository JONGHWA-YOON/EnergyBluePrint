#pragma once

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

namespace YunuD2D
{
    struct Pixel;
}
namespace yunutyEngine
{
    namespace D2D
    {

        struct YUNUTY_API YunutyPixel
        {
            YunutyPixel(const YunuD2D::Pixel& pixel);
            unsigned short int r;
            unsigned short int g;
            unsigned short int b;
            unsigned short int a;
        };
    };
}


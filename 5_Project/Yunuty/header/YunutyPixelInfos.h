#pragma once
#include <d2d1.h>
#include <vector>
#include "YunutyPixel.h"

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

using namespace std;
using namespace YunuD2D;
namespace YunuD2D
{
    struct PixelInfos;
}
namespace yunutyEngine
{
    namespace D2D
    {
        class YunuD2DGraphicCore;
        class YUNUTY_API YunutyPixelInfos
        {
        public:
            YunutyPixelInfos(const PixelInfos& pixelInfos);
            int width;
            int height;
            vector<vector<YunutyPixel>> pixels;
            vector<YunutyPixel>& operator[](int index) { return pixels[index]; };
            const vector<YunutyPixel>& operator[](int index) const { return pixels[index]; };
            virtual ~YunutyPixelInfos() {}
        };
    }
}

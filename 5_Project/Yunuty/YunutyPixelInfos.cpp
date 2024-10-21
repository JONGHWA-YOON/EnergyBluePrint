#include "YunuD2DGraphicCore.h"
#include "YunutyPixelInfos.h"
using namespace std;
using namespace YunutyEngine::D2D;

YunutyPixelInfos::YunutyPixelInfos(const PixelInfos& pixelInfos)
{
    width = pixelInfos.width;
    height = pixelInfos.height;
    for (int y = 0; y < pixelInfos.height; y++)
    {
        pixels.push_back(vector<YunutyPixel>());
        for (int x = 0; x < pixelInfos.width; x++)
            pixels[y].push_back(YunutyPixel(pixelInfos.pixels[y][x]));
    }
}
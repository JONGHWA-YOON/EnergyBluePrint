#include "pch.h"
#include "YunutyD2DPixelInfos.h"

using namespace YunuD2D;

YunutyD2DPixelInfos::YunutyD2DPixelInfos(ID2D1Bitmap* pBitmap)
{
    rect = D2D1::RectU(0, 0, pBitmap->GetPixelSize().width, pBitmap->GetPixelSize().height);
    pPixels = new UINT32[rect.right, rect.bottom];
    pBitmap->CopyFromMemory(&rect, pPixels, 4 * rect.right);
    width = rect.right;
    height = rect.bottom;

    for (UINT y = 0; y < rect.bottom; y++)
    {
        pixels.push_back(vector<Pixel>());
        for (UINT x = 0; x < rect.right; x++)
            pixels[y].push_back(Pixel(pPixels[y * rect.right + x]));
    }

    delete[] pPixels;
}
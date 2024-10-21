#pragma once
#include <Windows.h>
#include <vector>
#include <unordered_set>
#include "CanvasRenderSpace.h"
#include "Component.h"
#include "Vector2.h"
#include "Rect.h"
#include <d2d1.h>
#include <d2d1_1.h>


#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

using namespace std;

namespace YunutyEngine
{
    namespace D2D
    {
        class D2DCamera;
        class D2DCycle;
        class YunutyPixelInfos;
        class YUNUTY_API D2DGraphic : public Component
        {
            friend D2DCamera;
            friend D2DCycle;
        private:
            //static ID2D1Factory* direct2DFactory;
            static ID2D1HwndRenderTarget* HwndRenderTarget;
            static unordered_set<D2DGraphic*> D2DGraphics[(int)(CanvasRenderSpace::WorldSpace)+1];
            CanvasRenderSpace renderSpace = CanvasRenderSpace::NullSpace;
        protected:
            D2DGraphic();
            virtual void Render(D2D1::Matrix3x2F transform) = 0;
            Rect drawRect;
        public:
            D2D1::ColorF color = D2D1::ColorF::White;
            virtual ~D2DGraphic();
            //static const unordered_set<D2DGraphic*>& GetD2DGraphics();
            CanvasRenderSpace GetRenderSpace() { return renderSpace; }
            void SetRenderSpace(CanvasRenderSpace renderSpace);
        };
        YUNUTY_API YunutyPixelInfos GetPixelInfos(wstring imgFilepath);
    }
}

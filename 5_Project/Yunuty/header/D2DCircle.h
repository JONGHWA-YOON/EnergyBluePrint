#pragma once
#include <Windows.h>
#include <vector>
#include <unordered_set>
#include "Vector2.h"
#include "Rect.h"
#include "D2DGraphic.h"


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
        class D2DCamera2D;
        class YUNUTY_API D2DCircle :
            public D2DGraphic
        {
        private:
        protected:
            virtual void Render(D2D1::Matrix3x2F transform) ;
        public:
            float radius=1;
            D2D1::ColorF color=D2D1::ColorF::Gray;
            float border=1;
            bool filled = true;
        };
    }
}

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
        class YUNUTY_API D2DText :
            public D2DGraphic
        {
        private:
        protected:
            virtual void Render(D2D1::Matrix3x2F transform);
        public:
            wstring text;
            double fontSize=10;
            double width=100;
            double height=20;
        };
    }
}

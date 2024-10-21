#pragma once
#include <Windows.h>
#include <vector>
#include <unordered_set>
#include "Graphic.h"
#include "Vector2.h"
#include "Rect.h"
#include "WinAPIGraphic.h"


using namespace std;
namespace YunutyEngine
{
    namespace WinAPI
    {
        class WinAPICamera2D;
        class WinAPICircle :
            public WinAPIGraphic
        {
        private:
        protected:
            virtual void Update()override;
            virtual void Render(HDC hdc, const Vector2d position, const Vector2d& scale = Vector2d::one, double angle = 0) override;
        public:
        };
    }
}

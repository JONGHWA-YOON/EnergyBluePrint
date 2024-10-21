#pragma once

#include <Windows.h>
#include "Object.h"
#include "Camera.h"
#include "Component.h"
#include "Vector2.h"

using namespace YunutyEngine;

namespace YunutyEngine
{
    namespace WinAPI
    {
        class WinAPICycle;
        class WinAPICamera2D :
            public Camera
        {
        private:
            int resolutionW = 1920;
            int resolutionH = 1080;
            HBITMAP hBit = 0;
            HDC hdc = 0;
            HDC memDc = 0;
            void InitHDC(HWND hWnd);

        protected:
            virtual void Render() override;
            LRESULT CALLBACK Render(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
        public:
            Vector2d resolutionFactor = Vector2d::one;
            friend YunutyCycle;
            friend WinAPICycle;
        };
    }
}

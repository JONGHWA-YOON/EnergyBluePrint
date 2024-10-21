#pragma once

#include <Windows.h>
#include "Object.h"
#include "Camera.h"
#include "Component.h"
#include "Vector2.h"
#include "D2DGraphic.h"

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

using namespace YunutyEngine;

namespace YunutyEngine
{
    namespace D2D
    {
        class D2DCycle;
        class YUNUTY_API D2DCamera :
            public Camera
        {
        private:
            // resolution defines the rect of area shown by camera
            double resolutionW = 1920;
            double resolutionH = 1080;
            double zoomOutFactor = 1;
            D2D1::Matrix3x2F ScaleTransform(float x, float y);
            D2D1::Matrix3x2F RotationTransform(float angle);
            D2D1::Matrix3x2F TranslationTransform(float dx, float dy);

        protected:
            virtual void Render() override;
            LRESULT CALLBACK Render(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
        public:
            void SetZoomOutFactor(double zoomOutFactor);
            double GetZoomOutFactor() { return this->zoomOutFactor; }
            Rect GetResolution() { return Rect(resolutionW, resolutionH); }
            static D2DCamera* GetMainD2DCamera();
            friend YunutyCycle;
            friend D2DCycle;
        };
    }
}

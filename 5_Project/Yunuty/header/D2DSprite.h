#pragma once
#include "D2DGraphic.h"
#include "YunutyEngine.h"

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

using namespace YunutyEngine::D2D;

namespace YunutyEngine
{
    namespace D2D
    {
        class YUNUTY_API D2DSprite : public D2DGraphic
        {
        private:
            virtual void Render(D2D1::Matrix3x2F transform) override;
            wstring spriteFilePath=L"";
            Rect drawRect = Rect(0,0);
        public:
            void SetSpriteFilePath(const wstring& spriteFilePath,double* width=nullptr,double* height=nullptr);
            wstring GetSpriteFilePath() { return spriteFilePath; }
            void SetDrawRect(const Rect& drawRect) { this->drawRect = drawRect; }
            Rect GetDrawRect() { return drawRect; }
        };
    }
}

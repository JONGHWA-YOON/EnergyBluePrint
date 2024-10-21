#pragma once
#include <Windows.h>
#include <vector>
#include <unordered_set>
#include "Vector2.h"
#include "Rect.h"


using namespace std;
namespace yunutyEngine
{
    namespace WinAPI
    {
        class WinAPICamera2D;
        class WinAPIGraphic :
            public Component
        {
            friend WinAPICamera2D;
        private:
            static unordered_set<WinAPIGraphic*> D2DGraphics;
        protected:
            // position은 Yunuty 공간상의 게임오브젝트 위치가 아닌 winAPI에서 그래픽을 찍을때의 위치에 해당합니다.
            // Scale에 따라 그리는 그래픽의 크기가 달라져야 합니다. Scale의 크기가 음수일 경우 모양이 반전됩니다.
            // 이 함수의 매개변수 scale은 transform의 스케일과는 별개입니다.
            // angle은 그래픽 요소가 향하는 동경 각도를 의미합니다. 0도는 오른쪽을, 90도는 위쪽을 향하게 됩니다.
            virtual void RenderToTexture(HDC hdc,const Vector2d position, const Vector2d& scale = Vector2d::one, double angle = 0) = 0;
            Rect drawRect;
        public:
            WinAPIGraphic();
            virtual ~WinAPIGraphic();
            static const unordered_set<WinAPIGraphic*>& GetWinAPIGraphics();
        };
    }
}

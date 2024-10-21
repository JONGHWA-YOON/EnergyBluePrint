#pragma once
#include <Windows.h>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "Input.h"
#include "Vector2.h"
#include "Rect.h"


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
        class D2DCycle;
        class YUNUTY_API Input :
            public Input
        {
            friend D2DCycle;
        private:
            unordered_set<KeyCode> keyDownSet;
            unordered_set<KeyCode> keyPushedSet;
            unordered_set<KeyCode> keyLiftedSet;
            unordered_map<unsigned int, KeyCode> rawKeyEnumMap;
            static Input* instance;
            Input();
            LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
        protected:
            virtual ~Input();
            virtual void Update()override;
        public:
            virtual bool m_isKeyDown(KeyCode keyCode);
            virtual bool m_isKeyPushed(KeyCode keyCode);
            virtual bool m_isKeyLifted(KeyCode keyCode);
            static Vector2d getMouseWorldPosition();
        };
    }
}

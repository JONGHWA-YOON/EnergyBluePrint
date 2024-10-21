#pragma once
#include <Windows.h>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "Input.h"
#include "Vector2.h"
#include "Rect.h"


using namespace std;
namespace YunutyEngine
{
    namespace WinAPI
    {
        class WinAPICycle;
        class WinAPIInput :
            public Input
        {
            friend WinAPICycle;
        private:
            unordered_set<KeyCode> keyDownSet;
            unordered_set<KeyCode> keyPushedSet;
            unordered_map<unsigned int, KeyCode> rawKeyEnumMap;
            static WinAPIInput* instance;
            WinAPIInput();
            LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
        public:
            virtual bool m_isKeyDown(KeyCode keyCode);
            virtual bool m_isKeyPushed(KeyCode keyCode);
        };
    }
}

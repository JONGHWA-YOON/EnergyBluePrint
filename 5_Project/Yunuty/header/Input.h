#pragma once
#include "Object.h"
#include "EKeycode.h"
#include "GlobalComponent.h"
#include "Vector2.h"
#include <dinput.h>
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
namespace yunutyEngine
{
    class YUNUTY_API Input :
        public GlobalComponent
    {
    public:
        static bool isKeyDown(KeyCode keyCode);
        static bool isKeyPushed(KeyCode keyCode);
        static bool isKeyLifted(KeyCode keyCode);
        static Vector2d getMouseScreenPosition();
        static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    private:
        static Input* instance;
        static Input* GetInstance();
        unordered_set<KeyCode> keyDownSet;
        unordered_set<KeyCode> keyPushedSet;
        unordered_set<KeyCode> keyLiftedSet;
        unordered_map<unsigned int, KeyCode> rawKeyEnumMap;
        Input();
        bool m_isKeyDown(KeyCode keyCode);
        bool m_isKeyPushed(KeyCode keyCode);
        bool m_isKeyLifted(KeyCode keyCode);
        virtual ~Input();
    protected:
        virtual void Update()override;
    };
}

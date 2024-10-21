#include "YunutyEngine.h"
#include "WinAPIInput.h"

using namespace YunutyEngine;
using namespace YunutyEngine::WinAPI;

WinAPIInput* WinAPIInput::instance = nullptr;

WinAPIInput::WinAPIInput()
{
    instance = this;
    rawKeyEnumMap.insert(make_pair(VK_LEFT, KeyCode::LeftArrow));
    rawKeyEnumMap.insert(make_pair(VK_RIGHT, KeyCode::RightArrow));
    rawKeyEnumMap.insert(make_pair(VK_UP, KeyCode::UpArrow));
    rawKeyEnumMap.insert(make_pair(VK_DOWN, KeyCode::DownArrow));
    rawKeyEnumMap.insert(make_pair(VK_SPACE, KeyCode::Space));
    rawKeyEnumMap.insert(make_pair(VK_RETURN, KeyCode::Return));
    rawKeyEnumMap.insert(make_pair('A', KeyCode::A));
    rawKeyEnumMap.insert(make_pair('B', KeyCode::B));
    rawKeyEnumMap.insert(make_pair('a', KeyCode::a));
    rawKeyEnumMap.insert(make_pair('b', KeyCode::b));

}
LRESULT CALLBACK WinAPIInput::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    keyPushedSet.clear();
    switch (message)
    {
    case WM_KEYDOWN:
        for (auto each : rawKeyEnumMap)
        {
            if (each.first==wParam)
            {
                if (keyDownSet.find(each.second) == keyDownSet.end())
                    keyPushedSet.insert(each.second);
                keyDownSet.insert(each.second);
            }
            else
            {
                keyDownSet.erase(each.second);
            }
        }
        break;
    }
    return 0;
}
bool WinAPIInput::m_isKeyDown(KeyCode keyCode)
{
    return keyDownSet.find(keyCode) != keyDownSet.end();
}
bool WinAPIInput::m_isKeyPushed(KeyCode keyCode)
{
    return keyPushedSet.find(keyCode) != keyPushedSet.end();
}

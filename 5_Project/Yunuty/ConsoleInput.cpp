#include "YunutyEngine.h"
#include "ConsoleInput.h"
#include "EKeycode.h"
#include <Windows.h>
//#include <WinUser.h>

using namespace YunutyEngine;
using namespace Console;

YunutyEngine::Console::ConsoleInput::ConsoleInput()
{
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
	rawKeyEnumMap.insert(make_pair(VK_LBUTTON, KeyCode::MouseLeftClick));
	rawKeyEnumMap.insert(make_pair(VK_MBUTTON, KeyCode::MouseMiddleClick));
	rawKeyEnumMap.insert(make_pair(VK_RBUTTON, KeyCode::MouseRightClick));
}

void YunutyEngine::Console::ConsoleInput::Update()
{
    keyPushedSet.clear();
    for (auto each : rawKeyEnumMap)
    {
        if (GetKeyState(each.first) & 0x8000)
        {
            //keyPushedSet.insert(each.second);
            if (keyDownSet.find(each.second) == keyDownSet.end())
                keyPushedSet.insert(each.second);
            keyDownSet.insert(each.second);
        }
        else
        {
            keyDownSet.erase(each.second);
        }
    }
}
bool YunutyEngine::Console::ConsoleInput::m_isKeyDown(KeyCode keyCode)
{
    return keyDownSet.find(keyCode) != keyDownSet.end();
}
bool YunutyEngine::Console::ConsoleInput::m_isKeyPushed(KeyCode keyCode)
{
    return keyPushedSet.find(keyCode) != keyPushedSet.end();
}

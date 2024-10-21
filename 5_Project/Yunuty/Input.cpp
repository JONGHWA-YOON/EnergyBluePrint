#include "YunutyEngine.h"
#include "Input.h"

using namespace yunutyEngine;

Input* Input::instance = nullptr;
Input* yunutyEngine::Input::GetInstance()
{
    if (instance == nullptr)
        instance = new yunutyEngine::Input();
    return Input::instance;
}
Input::Input()
{
    instance = this;
    rawKeyEnumMap.insert(make_pair(VK_ESCAPE, KeyCode::ESC));
    rawKeyEnumMap.insert(make_pair(VK_LEFT, KeyCode::LeftArrow));
    rawKeyEnumMap.insert(make_pair(VK_RIGHT, KeyCode::RightArrow));
    rawKeyEnumMap.insert(make_pair(VK_UP, KeyCode::UpArrow));
    rawKeyEnumMap.insert(make_pair(VK_DOWN, KeyCode::DownArrow));
    rawKeyEnumMap.insert(make_pair(VK_SPACE, KeyCode::Space));
    rawKeyEnumMap.insert(make_pair(VK_RETURN, KeyCode::Return));
    rawKeyEnumMap.insert(make_pair(VK_LBUTTON, KeyCode::MouseLeftClick));
    rawKeyEnumMap.insert(make_pair(VK_MBUTTON, KeyCode::MouseMiddleClick));
    rawKeyEnumMap.insert(make_pair(VK_RBUTTON, KeyCode::MouseRightClick));

    rawKeyEnumMap.insert(make_pair(VK_CONTROL, KeyCode::Control));
    rawKeyEnumMap.insert(make_pair(VK_LSHIFT, KeyCode::LShift));
    rawKeyEnumMap.insert(make_pair(VK_RSHIFT, KeyCode::RShift));
    rawKeyEnumMap.insert(make_pair(VK_MENU, KeyCode::Alt));
    rawKeyEnumMap.insert(make_pair(VK_TAB, KeyCode::Tab));
    rawKeyEnumMap.insert(make_pair(VK_BACK, KeyCode::Backspace));
    rawKeyEnumMap.insert(make_pair(VK_DELETE, KeyCode::Delete));
    rawKeyEnumMap.insert(make_pair(VK_INSERT, KeyCode::Insert));
    rawKeyEnumMap.insert(make_pair(VK_HOME, KeyCode::Home));
    rawKeyEnumMap.insert(make_pair(VK_END, KeyCode::End));
    rawKeyEnumMap.insert(make_pair(VK_PRIOR, KeyCode::PgUp));
    rawKeyEnumMap.insert(make_pair(VK_NEXT, KeyCode::PgDn));

    rawKeyEnumMap.insert(make_pair('A', KeyCode::A));
    rawKeyEnumMap.insert(make_pair('B', KeyCode::B));
    rawKeyEnumMap.insert(make_pair('C', KeyCode::C));
    rawKeyEnumMap.insert(make_pair('D', KeyCode::D));
    rawKeyEnumMap.insert(make_pair('E', KeyCode::E));
    rawKeyEnumMap.insert(make_pair('F', KeyCode::F));
    rawKeyEnumMap.insert(make_pair('G', KeyCode::G));
    rawKeyEnumMap.insert(make_pair('H', KeyCode::H));
    rawKeyEnumMap.insert(make_pair('I', KeyCode::I));
    rawKeyEnumMap.insert(make_pair('J', KeyCode::J));
    rawKeyEnumMap.insert(make_pair('K', KeyCode::K));
    rawKeyEnumMap.insert(make_pair('L', KeyCode::L));
    rawKeyEnumMap.insert(make_pair('M', KeyCode::M));
    rawKeyEnumMap.insert(make_pair('N', KeyCode::N));
    rawKeyEnumMap.insert(make_pair('O', KeyCode::O));
    rawKeyEnumMap.insert(make_pair('P', KeyCode::P));
    rawKeyEnumMap.insert(make_pair('Q', KeyCode::Q));
    rawKeyEnumMap.insert(make_pair('R', KeyCode::R));
    rawKeyEnumMap.insert(make_pair('S', KeyCode::S));
    rawKeyEnumMap.insert(make_pair('T', KeyCode::T));
    rawKeyEnumMap.insert(make_pair('U', KeyCode::U));
    rawKeyEnumMap.insert(make_pair('V', KeyCode::V));
    rawKeyEnumMap.insert(make_pair('W', KeyCode::W));
    rawKeyEnumMap.insert(make_pair('X', KeyCode::X));
    rawKeyEnumMap.insert(make_pair('Y', KeyCode::Y));
    rawKeyEnumMap.insert(make_pair('Z', KeyCode::Z));

    rawKeyEnumMap.insert(make_pair('1', KeyCode::NUM_1));
    rawKeyEnumMap.insert(make_pair('2', KeyCode::NUM_2));
    rawKeyEnumMap.insert(make_pair('3', KeyCode::NUM_3));
    rawKeyEnumMap.insert(make_pair('4', KeyCode::NUM_4));
    rawKeyEnumMap.insert(make_pair('5', KeyCode::NUM_5));
    rawKeyEnumMap.insert(make_pair('6', KeyCode::NUM_6));
    rawKeyEnumMap.insert(make_pair('7', KeyCode::NUM_7));
    rawKeyEnumMap.insert(make_pair('8', KeyCode::NUM_8));
    rawKeyEnumMap.insert(make_pair('9', KeyCode::NUM_9));
    rawKeyEnumMap.insert(make_pair('0', KeyCode::NUM_0));

    rawKeyEnumMap.insert(make_pair(VK_NUMPAD1, KeyCode::NUMPAD_1));
    rawKeyEnumMap.insert(make_pair(VK_NUMPAD2, KeyCode::NUMPAD_2));
    rawKeyEnumMap.insert(make_pair(VK_NUMPAD3, KeyCode::NUMPAD_3));
    rawKeyEnumMap.insert(make_pair(VK_NUMPAD4, KeyCode::NUMPAD_4));
    rawKeyEnumMap.insert(make_pair(VK_NUMPAD5, KeyCode::NUMPAD_5));
    rawKeyEnumMap.insert(make_pair(VK_NUMPAD6, KeyCode::NUMPAD_6));
    rawKeyEnumMap.insert(make_pair(VK_NUMPAD7, KeyCode::NUMPAD_7));
    rawKeyEnumMap.insert(make_pair(VK_NUMPAD8, KeyCode::NUMPAD_8));
    rawKeyEnumMap.insert(make_pair(VK_NUMPAD9, KeyCode::NUMPAD_9));
    rawKeyEnumMap.insert(make_pair(VK_NUMPAD0, KeyCode::NUMPAD_0));

    rawKeyEnumMap.insert(make_pair(VK_OEM_PERIOD, KeyCode::Perido));
    rawKeyEnumMap.insert(make_pair(VK_OEM_PLUS, KeyCode::Plus));
    rawKeyEnumMap.insert(make_pair(VK_OEM_COMMA, KeyCode::Comma));
    rawKeyEnumMap.insert(make_pair(VK_OEM_MINUS, KeyCode::Minus));
    rawKeyEnumMap.insert(make_pair(VK_OEM_2, KeyCode::Slash));
    rawKeyEnumMap.insert(make_pair(VK_OEM_5, KeyCode::BackSlash));

}
Input::~Input()
{

}
void Input::Update()
{
    keyPushedSet.clear();
    keyLiftedSet.clear();
    for (auto each : rawKeyEnumMap)
    {
        if (GetKeyState(each.first) & 0x8000)
        {
            //if (isWindowFocused())
                //continue;
            if (keyDownSet.find(each.second) == keyDownSet.end())
                keyPushedSet.insert(each.second);
            keyDownSet.insert(each.second);
        }
        else
        {
            if (keyDownSet.find(each.second) != keyDownSet.end())
                keyLiftedSet.insert(each.second);
            keyDownSet.erase(each.second);
        }
    }
}
bool Input::m_isKeyDown(KeyCode keyCode)
{
    return keyDownSet.find(keyCode) != keyDownSet.end();
}
bool Input::m_isKeyPushed(KeyCode keyCode)
{
    return keyPushedSet.find(keyCode) != keyPushedSet.end();
}
bool Input::m_isKeyLifted(KeyCode keyCode)
{
    return keyLiftedSet.find(keyCode) != keyLiftedSet.end();
}
Vector2d yunutyEngine::Input::getMouseScreenPosition()
{
    POINT point;
    GetCursorPos(&point);
    return Vector2d(point.x, point.y);
}

// 해당 키코드와 일치하는 키가 눌러져 있다면 참을, 눌러져 있지 않다면 거짓을 반환합니다.
// 키가 계속 눌려져 있는 상태라면 매 프레임마다 참을 반환합니다.
bool yunutyEngine::Input::isKeyDown(KeyCode keyCode)
{
    return GetInstance()->m_isKeyDown(keyCode);
}
// 해당 키코드와 일치하는 키가 새로이 갓 눌러졌다면 참을, 그렇지 않다면 거짓을 반환합니다.
// 키가 계속 눌려져 있는 상태일지라도 키가 처음 눌러진 순간의 프레임에서만 참을 반환합니다.
bool yunutyEngine::Input::isKeyPushed(KeyCode keyCode)
{
    return GetInstance()->m_isKeyPushed(keyCode);
}
// 해당 키코드와 일치하는 키가 갓 떼어졌다면 참을, 그렇지 않다면 거짓을 반환합니다.
// 키가 계속 떼어져 있는 상태일지라도 키가 처음 떼어진 순간의 프레임에서만 참을 반환합니다.
bool yunutyEngine::Input::isKeyLifted(KeyCode keyCode)
{
    return GetInstance()->m_isKeyLifted(keyCode);
}
LRESULT CALLBACK yunutyEngine::Input::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return ::DefWindowProc(hWnd, message, wParam, lParam);
}

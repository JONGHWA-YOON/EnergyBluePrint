#include "YunuD2DGraphicCore.h"
#include "YunutyEngine.h"
#include "D2DInput.h"

using namespace YunutyEngine;
using namespace YunutyEngine::D2D;

Input* Input::instance = nullptr;

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
            if (!D2DCycle::GetInstance().isWindowFocused())
                continue;
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
LRESULT CALLBACK Input::MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    /*keyPushedSet.clear();
    switch (message)
    {
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_KEYDOWN:
        for (auto each : rawKeyEnumMap)
        {
            if (each.first == wParam)
            {
                if (keyDownSet.find(each.second) == keyDownSet.end())
                    keyPushedSet.insert(each.second);
                keyDownSet.insert(each.second);
            }
        }
        break;
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
        break;
    case WM_KEYUP:
        for (auto each : rawKeyEnumMap)
        {
            if (each.first == wParam)
            {
                keyDownSet.erase(each.second);
            }
        }
        break;
    }*/
    return 0;
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

Vector2d Input::getMouseWorldPosition()
{
    auto ret = getMousePositionNormalized();
    ret.x -= 0.5;
    ret.y -= 0.5;
    ret.y *= -1;
    auto camRes = D2DCamera::GetMainD2DCamera()->GetResolution();
    ret.x *= camRes.width * D2DCamera::GetMainD2DCamera()->GetZoomOutFactor();
    ret.y *= camRes.height * D2DCamera::GetMainD2DCamera()->GetZoomOutFactor();
    ret += D2DCamera::GetMainD2DCamera()->GetTransform()->GetWorldPosition();
    return ret;
}

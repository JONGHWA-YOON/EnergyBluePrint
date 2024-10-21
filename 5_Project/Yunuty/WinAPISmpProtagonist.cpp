#include "YunutyEngine.h"
#include "WinAPISmpProtagonist.h"
#include "Vector2.h"
#include "Input.h"
#include "EKeycode.h"

using namespace YunutyEngine;
using namespace YunutyEngine::WinAPI;
using namespace YunutyEngine::WinAPI::Sample;
void WinAPISmpProtagonist::Update()
{
    Vector2d direction = Vector2d::zero;
    if (Input::isKeyDown(KeyCode::DownArrow))
        direction += Vector2d::down;
    if (Input::isKeyDown(KeyCode::UpArrow))
        direction += Vector2d::up;
    if (Input::isKeyDown(KeyCode::LeftArrow))
        direction += Vector2d::left;
    if (Input::isKeyDown(KeyCode::RightArrow))
        direction += Vector2d::right;
    //direction = direction.Normalized();
}

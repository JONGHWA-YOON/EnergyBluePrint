#include "UIText.h"
#include "YunutyEngine.h"
#include "_YunuGIObjects.h"
using namespace yunutyEngine::graphics;

yunutyEngine::graphics::UIText::UIText() :
	Renderable<yunuGI::IUIText>(_YunuGIObjects::SingleInstance().factory->CreateUIText({}))
{
}
void yunutyEngine::graphics::UIText::Update()
{
	GetGI().SetWorldTM(GetTransform()->GetWorldTM());
}
module;
#include <d3d11.h>
#include "imgui.h"
#include "YunutyEngine.h"
#include "SelectManager.h"
#include "DebugManager.h"
module ui.ScenePanel;
import utility.engineGraphics;

void ScenePanel::Init()
{

}

void ScenePanel::Shutdown()
{

}

void ScenePanel::OnUpdate(float ts)
{

}

void ScenePanel::DrawPanel()
{
	ImGui::Begin("Scene View");
	
	std::string playText = "Play Game";
	ImVec2 textSize = ImGui::CalcTextSize(playText.c_str());
	ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - (textSize.x+6)) / 2);

	if (ImGui::Button(playText.c_str()))
	{
		if (DebugManager::GetInstance()->m_isPlayMode == false)
		{
			DebugManager::GetInstance()->m_isPlayMode = true;
		}
		else
		{
			DebugManager::GetInstance()->m_isPlayMode = false;
		}
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	m_gameImageSizeX = 1600.0f;
	m_gameImageSizeY = 900.0f;
	ImGui::Image(utility::engineGraphics::GetRenderedOutput(), { m_gameImageSizeX,m_gameImageSizeY });

	ReturnMousePos();

	ImGui::End();
}

void ScenePanel::ReturnMousePos()
{
	ImVec2 tempMousePos;
	tempMousePos.x = ImGui::GetMousePos().x - ImGui::GetWindowPos().x - ImGui::GetCursorStartPos().x;
	tempMousePos.y = ImGui::GetMousePos().y - ImGui::GetWindowPos().y - ImGui::GetCursorStartPos().y;

	ImVec2 tempPanelSize;
	tempPanelSize.x = m_gameImageSizeX;
	tempPanelSize.y = m_gameImageSizeY;

	// 왼쪽 영역 밖으로 나갔을때 음수로 들어오므로 0으로 처리해버림
	if (tempMousePos.x < 0 || tempMousePos.y < 0)
	{
		tempMousePos.x = 0;
		tempMousePos.y = 0;
	}

	// 오른쪽 영역 밖으로 나갔을때도 0으로 처리
	else if (tempMousePos.x > tempPanelSize.x || tempMousePos.y > tempPanelSize.y)
	{
		tempMousePos.x = 0;
		tempMousePos.y = 0;
	}

	else
	{
		if (ImGui::IsMouseClicked(0))
		{
			auto nowObject = yunutyEngine::graphics::Renderer::SingleInstance().ClickRenderable(tempMousePos.x / tempPanelSize.x, tempMousePos.y / tempPanelSize.y);
			if (nowObject != nullptr)
			{
				SelectManager::GetInstance()->SetRecentHierarchy(nowObject->GetGameObject());
			}
			else
			{
				SelectManager::GetInstance()->SetRecentHierarchy(nullptr);
			}
		}

	}
}

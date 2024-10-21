module;
#include <d3d11.h>
//#include "imgui_internal.h"
//#include "backends/imgui_impl_vulkan.h"
#include "YunutyEngine.h"
//#include "Walnut/Application.h"
//#include "Walnut/Image.h"
//#include "vulkan/vulkan.h"
//#include "vulkan/vulkan_win32.h"
#include "imgui.h"
#include "DebugManager.h"

module ui.GameViewPanel;
import ui.DebugPanel;
import utility.engineGraphics;
import graphics.Camera;
import graphics.CallbackHandler;

void GameViewPanel::Init()
{
    //graphics::CallbackHandler::
        //ImGui::FindWindowByName("Game View")->DC.h;
}

void GameViewPanel::Shutdown()
{

}

void GameViewPanel::OnUpdate(float ts)
{

}

void GameViewPanel::DrawPanel()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

    ImGui::SetNextWindowBgAlpha(0.0f); // Make the background transparent
    ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(2560.0f, 1440.0f));

	ImGui::Begin("Game View", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

	m_gameImageSizeY = 1440.0f;
	m_gameImageSizeX = 2560.0f;

    ImGui::Image(utility::engineGraphics::GetRenderedOutput(), { m_gameImageSizeX,m_gameImageSizeY });

    ReturnMousePos();

    ImGui::End();

	ImGui::PopStyleVar(3);
}

void GameViewPanel::ReturnMousePos()
{
    //if (ImGui::IsMouseClicked(0))
    //{
    ImVec2 tempMousePos;
    tempMousePos.x = ImGui::GetMousePos().x - ImGui::GetWindowPos().x - ImGui::GetCursorStartPos().x;
    tempMousePos.y = ImGui::GetMousePos().y - ImGui::GetWindowPos().y - ImGui::GetCursorStartPos().y;

    ImVec2 tempPanelSize;
    tempPanelSize.x = m_gameImageSizeX;
    tempPanelSize.y = m_gameImageSizeY;
    //tempPanelSize.x = ImGui::GetContentRegionMax().x - ImGui::GetCursorStartPos().x;
    //tempPanelSize.y = ImGui::GetContentRegionMax().y - 2 * ImGui::GetCursorStartPos().y;

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
        //std::stringstream sstream;
        //sstream << "mouse x position : " << tempMousePos.x << std::endl
        //	<< "mouse y position : " << tempMousePos.y;
            //yunutyEngine::graphics::Renderer::SingleInstance().ClickRenderable(tempMousePos.x / tempPanelSize.x, tempMousePos.y / tempPanelSize.y);

        if (ImGui::IsMouseClicked(0))
            yunutyEngine::YunutyCycle::SingleInstance().ReserveActionAfterUpdate(
                [=]()
                {
                    yunutyEngine::graphics::Renderer::SingleInstance().InvokeClickEvent(tempMousePos.x / tempPanelSize.x, tempMousePos.y / tempPanelSize.y);
                });
        else
            yunutyEngine::YunutyCycle::SingleInstance().ReserveActionAfterUpdate(
                [=]()
                {
                    yunutyEngine::graphics::Renderer::SingleInstance().InvokeHoverEvent(tempMousePos.x / tempPanelSize.x, tempMousePos.y / tempPanelSize.y);
                });

        //DebugPanel::LogMessage(sstream.str().c_str());
    }
    //}
}
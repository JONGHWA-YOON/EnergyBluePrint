module;
#include "DebugManager.h"
#include "imgui.h"
#include <iostream>
#include <mutex>

module ui.DebugPanel;

std::vector<std::string> DebugPanel::logs;
std::mutex DebugPanel::logMutex;

void DebugPanel::Init()
{

}

void DebugPanel::Shutdown()
{

}

void DebugPanel::OnUpdate(float ts)
{

}

void DebugPanel::DrawPanel()
{
    //std::scoped_lock(logMutex);
    std::scoped_lock lock(logMutex);
    ImGui::Begin("Debug");

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

    if (ImGui::Button("Clear"))
    {
        ClearLogs();
    }

    ImGui::PopStyleColor(3);

    for (auto itr = logs.rbegin(); itr != logs.rend(); itr++)
    {
        ImGui::Text(itr->c_str());
    }

    ImGui::End();
}

module;
#include <compare>
#include "imgui.h"
module ui.ProjectPanel;
import htkg.SaveLoad;

void ProjectPanel::Init()
{

}

void ProjectPanel::Shutdown()
{

}

void ProjectPanel::OnUpdate(float ts)
{

}

void ProjectPanel::DrawPanel()
{
	ImGui::Begin("Project");

	if (ImGui::Button("Save", ImVec2(150.0f, 0.0f)))
	{
		SaveLoad::SaveScene();
	}

	ImGui::SameLine();

	ImGui::Dummy(ImVec2(10.0f, 0.0f));

	ImGui::SameLine();

	if (ImGui::Button("Load", ImVec2(150.0f, 0.0f)))
	{
		SaveLoad::LoadScene();
	}
	ImGui::End();
}

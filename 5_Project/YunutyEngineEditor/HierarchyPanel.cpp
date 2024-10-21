module;
#include "YunutyEngine.h"
#include <vector>
#include "imgui.h"
// 헤더에 추가해놨는데 여기에 추가안하면 오류뜸
#include "imgui_internal.h"
#include "SelectManager.h"
#include "DebugManager.h"
import ui.InspectorPanel;
import ui.EditorPanel;
module ui.HierarchyPanel;

using namespace yunutyEngine;


void HierarchyPanel::Init()
{

}

void HierarchyPanel::Shutdown()
{

}

void HierarchyPanel::OnUpdate(float ts)
{

}

void HierarchyPanel::DrawPanel()
{
	ImGui::Begin("Hierarchy");

	for (int i = 0; i < Scene::getCurrentScene()->GetChildren().size(); i++)
	{
		auto nowObject = Scene::getCurrentScene()->GetChildren()[i];

		// 현재 오브젝트의 이름을 가져옴
		std::string tempString = nowObject->getName();

		CreateOneTreeNode(nowObject);
	}
	CalculateBlank();

	m_prevYcursor = ImGui::GetCursorPosY() + ImGui::GetWindowPos().y - 5;

	RightClick();

	std::vector<GameObject*> tempVec;
	m_ObjectList.swap(tempVec);

	std::vector<int> tempInt;
	m_IndexList.swap(tempInt);

	DebugManager::GetInstance()->DrawDebugText("Mouse : %f, %f", ImGui::GetMousePos().x - ImGui::GetWindowPos().x - ImGui::GetCursorStartPos().x, ImGui::GetMousePos().y - ImGui::GetWindowPos().y - ImGui::GetCursorStartPos().y);
	DebugManager::GetInstance()->DrawDebugText("Region : %f, %f", ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
	DebugManager::GetInstance()->DrawDebugText("Dragging : %d", m_isDragging);
	if (m_dragTarget != nullptr)
		DebugManager::GetInstance()->DrawDebugText("Drag target: %s", m_dragTarget->getName().c_str());

	ImGui::End();
}

// 스스로를 그리고, 자식이 있다면 자식도 그린다.
void HierarchyPanel::CreateOneTreeNode(yunutyEngine::GameObject* root)
{
	ImGuiTreeNodeFlags nodeFlag = ImGuiTreeNodeFlags_None;
	nodeFlag |= ImGuiTreeNodeFlags_OpenOnArrow;

	if (root->GetChildren().empty())
	{
		nodeFlag |= ImGuiTreeNodeFlags_Leaf;
	}

	/// 특정 객체를 선택하면 그 객체만 아닌 자식 객체도 모두 색칠되어 일단 주석처리
	if (root == SelectManager::GetInstance()->GetRecentHierarchy())
	{
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));          // 기본 색상
		//ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));   // 마우스 오버 시 색상
		//ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));    // 클릭 시 색
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
	}

	bool open;
	if (root->getName().empty() == true)
	{
		open = ImGui::TreeNodeEx(root, nodeFlag, "UnNamed Object");
	}
	else
	{
		open = ImGui::TreeNodeEx(root, nodeFlag, root->getName().c_str());
	}

	if (root == SelectManager::GetInstance()->GetRecentHierarchy())
	{
		ImGui::PopStyleColor(2);
	}

	m_ObjectList.push_back(root);

	//ImGui::Dummy(ImVec2(0, 10)); // 간격 조절
	ImGui::Spacing();

	ClickLogic(root);

	DragLogic(root);

	if (open)
	{
		for (yunutyEngine::GameObject* each : root->GetChildren())
		{
			CreateOneTreeNode(each);
		}
		ImGui::TreePop();
	}
}

void HierarchyPanel::RightClickPopUp(GameObject* obj)
{
	if (ImGui::IsItemClicked(1))
	{
		SelectManager::GetInstance()->SetRecentHierarchy(obj);
		m_popUpFlag = true;
	}

	if (m_popUpFlag == true)
	{
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Add Object"))
			{
				// 현재 씬에 오브젝트의 수를 정확히 가져올 수 없어 임시로 넣어놓음 -> 만들때마다 newObject0,1,2,3 숫자가 올라감
				std::string temp = "newObject";
				std::string num = std::to_string(m_NamingNum);
				std::string result = temp + num;
				m_NamingNum++;

				// 부모가 존재하는 오브젝트를 생성
				auto newGameObject = Scene::getCurrentScene()->AddGameObject(obj);
				newGameObject->setName(result);

				m_popUpFlag = false;
			}

			if (ImGui::MenuItem("Set Object Name"))
			{
				m_setNameFlag = true;
				m_popUpFlag = false;
			}

			if (ImGui::MenuItem("Delete Object"))
			{
				//SelectManager::GetInstance()->DeleteHierarchyList(obj);
				DeleteObject(obj);
			}

			ImGui::EndPopup();
		}
	}

	if (m_setNameFlag == true && obj == SelectManager::GetInstance()->GetRecentHierarchy())
	{
		ImVec2 drawPos;
		drawPos.x = ImGui::GetMousePos().x - ImGui::GetWindowPos().x - ImGui::GetCursorStartPos().x + 100.0f;
		drawPos.y = ImGui::GetMousePos().y - ImGui::GetWindowPos().y - ImGui::GetCursorStartPos().y + 100.0f;

		ImGui::SetNextWindowPos(ImGui::GetMousePos(), ImGuiCond_Appearing);
		ImGui::OpenPopup("Change Name");

		ImGui::SetNextItemWidth(108);

		if (ImGui::BeginPopupModal("Change Name"))
		{
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
			ImGui::Text("%s", obj->getName().c_str());
			ImGui::Text("->");
			ImGui::SameLine();
			ImGui::InputText("##input", inputStringBuffer, IM_ARRAYSIZE(inputStringBuffer));
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
			if (ImGui::Button("  OK  "))
			{
				obj->setName(inputStringBuffer);
				m_setNameFlag = false;
				memset(inputStringBuffer, 0, sizeof(inputStringBuffer));
				ImGui::CloseCurrentPopup();
			}
			ImGui::PopStyleColor(5);

			ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
			if (ImGui::Button("Cancel"))
			{
				m_setNameFlag = false;
				memset(inputStringBuffer, 0, sizeof(inputStringBuffer));
				ImGui::CloseCurrentPopup();
			}
			ImGui::PopStyleColor(3);

			ImGui::EndPopup();
		}
	}
}

void HierarchyPanel::LeftClickSelected(GameObject* obj)
{
	if (ImGui::IsItemClicked(0))
	{
		//SelectManager::GetInstance()->InsertHierarchyList(obj);
		SelectManager::GetInstance()->SetRecentHierarchy(obj);
	}
}

void HierarchyPanel::DragLogic(GameObject* obj)
{
	if (ImGui::IsMouseDragging(0))
	{
		m_isDragging = true;
	}

	//if (ImGui::IsMouseDragging(0)) {
	//	if (!m_isDragging) {
	//		yunutyEngine::Time timer;
	//		m_dragTimer += timer.GetDeltaTime();
	//	}
	//
	//	if (m_dragTimer >= 5.0f) {
	//		m_isDragging = true; // 드래그가 1초 이상 지속될 때 m_isDragging을 true로 설정
	//	}
	//}
	//else {
	//	m_dragTimer = 0.0f;
	//	//m_isDragging = false;
	//}


	if (m_isDragging == true && ImGui::IsItemHovered())
	{
		m_dragTarget = obj;
		m_isDragging = false;
	}

	if (m_isDragging == false && m_dragTarget != nullptr && m_dragTarget != SelectManager::GetInstance()->GetRecentHierarchy() && ImGui::IsItemHovered())
	{
		if (SelectManager::GetInstance()->GetRecentHierarchy() != nullptr)
		{
			for (auto child : SelectManager::GetInstance()->GetRecentHierarchy()->GetChildren())
			{
				if (child == m_dragTarget)
				{
					if (SelectManager::GetInstance()->GetRecentHierarchy()->GetParentGameObject() != nullptr)
					{
						m_dragTarget->SetParent(SelectManager::GetInstance()->GetRecentHierarchy()->GetParentGameObject());
					}

					else
					{
						m_dragTarget->SetParent(Scene::getCurrentScene());
					}
					break;
				}
			}

			SelectManager::GetInstance()->GetRecentHierarchy()->SetParent(m_dragTarget);

			m_dragTarget = nullptr;
		}
	}

	if (ImGui::GetMousePos().y >= m_prevYcursor && ImGui::IsMouseReleased(0))
	{
		m_dragTarget = nullptr;
		m_isDragging = false;
	}

	if (m_isDragging == true && m_dragTarget != nullptr)
	{
		TextNearMouse(SelectManager::GetInstance()->GetRecentHierarchy()->getName());
	}
}

void HierarchyPanel::ClickLogic(GameObject* obj)
{
	LeftClickSelected(obj);
	RightClickPopUp(obj);
}

void HierarchyPanel::TextNearMouse(std::string text)
{
	ImVec2 originPos = ImGui::GetCursorPos();
	ImVec2 newPos;
	newPos.x = ImGui::GetMousePos().x - ImGui::GetWindowPos().x + 15;
	newPos.y = ImGui::GetMousePos().y - ImGui::GetWindowPos().y;
	ImGui::SetCursorPos(newPos);
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 0.5f), text.c_str(), newPos);

	ImGui::SetCursorPos(originPos);
}

void HierarchyPanel::RightClick()
{
	if (ImGui::GetMousePos().y >= m_prevYcursor && ImGui::IsMouseClicked(1) && ImGui::GetMousePos().x >= ImGui::GetWindowPos().x && ImGui::GetMousePos().x <= ImGui::GetWindowPos().x + ImGui::GetContentRegionAvail().x)
	{
		m_rightFlag = true;
	}

	if (ImGui::GetMousePos().y < m_prevYcursor && ImGui::IsMouseClicked(1))
	{
		m_rightFlag = false;
	}

	if (m_rightFlag == true)
	{

		ImGui::OpenPopup("Add");
		{
		}

		if (ImGui::BeginPopup("Add"))
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

			if (ImGui::Button("Add Object", ImVec2(100, 15)))
			{
				// 현재 씬에 오브젝트의 수를 정확히 가져올 수 없어 임시로 넣어놓음 -> 만들때마다 newObject0,1,2,3 숫자가 올라감
				std::string temp = "newObject";
				std::string num = std::to_string(m_NamingNum);
				std::string result = temp + num;
				m_NamingNum++;

				// 부모가 존재하는 오브젝트를 생성
				auto newGameObject = Scene::getCurrentScene()->AddGameObject();
				newGameObject->setName(result);

				m_rightFlag = false;
				ImGui::CloseCurrentPopup();
			}

			if ((ImGui::IsMouseClicked(0)) && !ImGui::IsItemHovered())
			{
				m_rightFlag = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::PopStyleColor();

			ImGui::EndPopup();
		}
	}
}

void HierarchyPanel::CalculateBlank()
{
	UINT treeNodeSize = 13;

	UINT blankSize = 8;

	ImVec2 startPos;

	startPos.x = ImGui::GetWindowPos().x + ImGui::GetCursorStartPos().x;

	startPos.y = ImGui::GetWindowPos().y + ImGui::GetCursorStartPos().y - blankSize;

	for (int index = 0; index < m_ObjectList.size() + 1; index++)
	{
		m_IndexList.push_back(treeNodeSize * index + startPos.y + blankSize * index);
	}

	for (int i = 0; i < m_IndexList.size(); i++)
	{
		if (ImGui::GetMousePos().y >= m_IndexList[i] && ImGui::GetMousePos().y < m_IndexList[i] + blankSize && ImGui::GetMousePos().x >= ImGui::GetWindowPos().x && ImGui::GetMousePos().x <= ImGui::GetWindowPos().x + ImGui::GetContentRegionAvail().x)
		{
			if (m_dragTarget == nullptr)
			{
				m_dragTarget = nullptr;
				m_isDragging = false;
			}
		}


		if (ImGui::GetMousePos().y >= m_IndexList[i] && ImGui::GetMousePos().y < m_IndexList[i] + blankSize && ImGui::GetMousePos().x >= ImGui::GetWindowPos().x && ImGui::GetMousePos().x <= ImGui::GetWindowPos().x + ImGui::GetContentRegionAvail().x)
		{
			{
				auto contentRegion = ImGui::GetContentRegionMax();
				ImVec2 windowPos = ImGui::GetWindowPos();

				float x = windowPos.x;
				float y = m_IndexList[i];
				float width = contentRegion.x;
				float height = blankSize - 1;
				ImU32 fillColor = IM_COL32(255, 255, 0, 63);

				ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + width, y + height), fillColor);

				if (m_dragTarget != nullptr && m_isDragging == true && ImGui::IsMouseReleased(0) && SelectManager::GetInstance()->GetRecentHierarchy() != nullptr)
				{
					m_dragTarget = nullptr;
					m_isDragging = false;
					yunutyEngine::GameObject* nowObject = nullptr;
					if (i > m_ObjectList.size() - 1)
					{
						nowObject = m_ObjectList.back();

						if (nowObject->GetParentGameObject())
						{
							auto parentObject = nowObject->GetParentGameObject();
							SelectManager::GetInstance()->GetRecentHierarchy()->SetParent(parentObject);
						}

						else
						{
							auto parentObject = nowObject->GetScene();
							SelectManager::GetInstance()->GetRecentHierarchy()->SetParent(parentObject);
						}

					}
					else
					{
						nowObject = m_ObjectList[i];

						if (nowObject->GetParentGameObject())
						{
							auto parentObject = nowObject->GetParentGameObject();
							SelectManager::GetInstance()->GetRecentHierarchy()->SetParent(parentObject);

							auto childVec = parentObject->GetChildren();
							auto iter = find(childVec.begin(), childVec.end(), nowObject);
							if (iter != childVec.end())
							{
								int index = std::distance(childVec.begin(), iter);
								SelectManager::GetInstance()->GetRecentHierarchy()->SetChildIndex(index);
							}
						}

						else
						{
							auto parentObject = nowObject->GetScene();
							SelectManager::GetInstance()->GetRecentHierarchy()->SetParent(parentObject);

							auto childVec = parentObject->GetChildren();
							auto iter = find(childVec.begin(), childVec.end(), nowObject);
							if (iter != childVec.end())
							{
								int index = std::distance(childVec.begin(), iter);
								SelectManager::GetInstance()->GetRecentHierarchy()->SetChildIndex(index);
							}
						}
					}
				}
			}
		}
	}
}

void HierarchyPanel::DeleteObject(GameObject* obj)
{
	SelectManager::GetInstance()->SetRecentHierarchy(nullptr);

	if (InspectorPanel::isAcitvateList.find(obj) != InspectorPanel::isAcitvateList.end())
	{
		InspectorPanel::isAcitvateList.erase(obj);
	}

	if (InspectorPanel::buttonTypeList.find(obj) != InspectorPanel::buttonTypeList.end())
	{
		InspectorPanel::buttonTypeList.erase(obj);
	}

	if (InspectorPanel::imageList.find(obj) != InspectorPanel::imageList.end())
	{
		InspectorPanel::imageList.erase(obj);
	}

	Scene::getCurrentScene()->DestroyGameObject(obj);

	m_popUpFlag = false;
}
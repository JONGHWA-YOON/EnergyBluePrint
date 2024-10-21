module;
#include "SelectManager.h"
#include "DebugManager.h"
#include "ButtonLogicManager.h"
#include "imgui.h"
#include "imfilebrowser.h"

module ui.InspectorPanel;
import ui.DebugPanel;
import test.components.HailKimJungIl;
import htkg.ComponentUI;
import htkg.InGame.Stage;
import htkg.InGame.Tile;
import htkg.InGame.StaticMeshWrapper;
import htkg.InGame.SkinnedMeshWrapper;
import JHImGui;

const int MAX_TEXT_PRINT = 14;

bool InspectorPanel::isAddComponetClick = false;

std::unordered_map<yunutyEngine::GameObject*, bool> InspectorPanel::isAcitvateList;
std::unordered_map<yunutyEngine::GameObject*, ButtonLogicManager::eButtonType> InspectorPanel::buttonTypeList;
std::unordered_map<yunutyEngine::GameObject*, std::string> InspectorPanel::imageList;


void InspectorPanel::Init()
{
	m_pButtonLogicManager = new ButtonLogicManager();
}

void InspectorPanel::Shutdown()
{
	delete m_pButtonLogicManager;
}

void InspectorPanel::OnUpdate(float ts)
{

}

void InspectorPanel::DrawPanel()
{
	// 디버그 플래그 코드 일단 여기 넣어놓음
	SetDebugFlag();

	m_enumFlagList.erase(m_enumFlagList.begin() + m_enumButtonIndex, m_enumFlagList.end());
	m_enumButtonIndex = 0;

	ImGui::Begin("Inspector");

	DrawNowObjectInfo();

	DebugManager::GetInstance()->DrawDebugText("Mouse : %f, %f", ImGui::GetMousePos().x - ImGui::GetWindowPos().x - ImGui::GetCursorStartPos().x, ImGui::GetMousePos().y - ImGui::GetWindowPos().y - ImGui::GetCursorStartPos().y);
	DebugManager::GetInstance()->DrawDebugText("Region : %f, %f", ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
	DebugManager::GetInstance()->DrawDebugText("ScreenPos : %f, %f", ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);
	DebugManager::GetInstance()->DrawDebugText("WindowPos : %f, %f", ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);

	ImGui::End();
}

template<typename DataType>
void InspectorPanel::DrawSpecificData(DataType& data, const std::string name)
{
	if constexpr (std::is_same_v<DataType, std::string>)
	{
		ImGui::Text(data.c_str());

		//auto nowObject = SelectManager::GetInstance()->GetRecentHierarchy();

		//ImGui::InputText(data.c_str(), inputStringBuffer, sizeof(inputStringBuffer));

		//if (ImGui::Button("Print Text"))
		//{
		//	ImGui::Text("You entered: %s", &stringList.at(nowObject));
		//}
		//
		//DebugManager::GetInstance()->DrawDebugText("string, %s", stringList.at(nowObject));
	}

	else
	{
		std::ostringstream addressStream;

		addressStream << &data;

		std::string titleName = name;

		int nameSize = titleName.size();
		if (nameSize <= MAX_TEXT_PRINT);
		{
			for (int i = nameSize; i < 15; i++)
			{
				titleName += " ";
			}
		}

		ImGui::Text(titleName.c_str());

		std::string nameID = "##";
		nameID += addressStream.str();
		nameID += name;

		ImGui::SameLine();
		ImGui::SetNextItemWidth(100);

		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

		ImGui::SetNextWindowContentSize(ImVec2(10, 3)); // 내용 영역 크기 조절

		// int 
		if constexpr (std::is_same_v<DataType, int>)
		{
			ImGui::InputInt(nameID.c_str(), &data, 1);
			ImGui::PopStyleColor(5);
			DebugManager::GetInstance()->DrawDebugText("int, %s : %d", name.c_str(), data);
		}

		// float
		else if constexpr (std::is_same_v<DataType, float>)
		{
			ImGui::InputFloat(nameID.c_str(), &data, 0.001f);
			ImGui::PopStyleColor(5);
			DebugManager::GetInstance()->DrawDebugText("float, %s : %f", name.c_str(), data);
		}

		// double
		else if constexpr (std::is_same_v<DataType, double>)
		{
			ImGui::InputFloat(nameID.c_str(), &data, 0.001f);
			ImGui::PopStyleColor(5);
			DebugManager::GetInstance()->DrawDebugText("double, %s : %f", name.c_str(), data);
		}

		// vector
		else if constexpr (std::is_same_v<DataType, yunutyEngine::Vector3d>)
		{
			int tempInt = 0;
			nameID += std::to_string(tempInt);
			ImGui::InputDouble(nameID.c_str(), &data.x, 0.001f, 0.0f, "%.3f");
			tempInt++;

			ImGui::SameLine();
			ImGui::SetNextItemWidth(100);
			nameID += std::to_string(tempInt);
			ImGui::InputDouble(nameID.c_str(), &data.y, 0.001f, 0.0f, "%.3f");
			tempInt++;

			ImGui::SameLine();
			ImGui::SetNextItemWidth(100);
			nameID += std::to_string(tempInt);
			ImGui::InputDouble(nameID.c_str(), &data.z, 0.001f, 0.0f, "%.3f");

			ImGui::PopStyleColor(5);

			DebugManager::GetInstance()->DrawDebugText("vector, %s : %f, %f, %f", name.c_str(), data.x, data.y, data.z);
		}
	}
}

// bool
template<>
void InspectorPanel::DrawSpecificData<bool>(bool& data, std::string name)
{
	int nameSize = name.size();
	if (nameSize <= MAX_TEXT_PRINT);
	{
		for (int i = nameSize; i < 15; i++)
		{
			name += " ";
		}
	}

	ImGui::Text("%s", name.c_str());

	// true 버튼
	ImGui::SameLine();

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(30, 1.5f));

	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));

	if (data == true)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
	}

	if (ImGui::JHButton(&data, "True"))
	{
		data = true;
	}

	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar(1);

	// false 버튼
	ImGui::SameLine();

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(30, 1.5f));

	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));

	if (data == false)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
	}

	if (ImGui::JHButton(&data + 1, "false"))
	{
		data = false;
	}

	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar(1);

	DebugManager::GetInstance()->DrawDebugText("bool, %s : %s", name.c_str(), data ? "True" : "False");
}

// enum class
template<typename enumType>
void InspectorPanel::DrawEnumClass(enumType& data, const vector<const char*>& symbols, const std::string name)
{
	static_assert(std::is_enum<enumType>::value, "enumType must be an enum class");

	m_enumFlagList.emplace_back(false);
	int value = static_cast<int>(data);

	if (value >= 0 && value < symbols.size())
	{
		std::string titleName = name;

		int nameSize = titleName.size();
		if (nameSize <= MAX_TEXT_PRINT);
		{
			for (int i = nameSize; i < 15; i++)
			{
				titleName += " ";
			}
		}

		ImGui::Text("%s %s", titleName.c_str(), symbols[value]);

		ImGui::SameLine();
		//ImGui::Dummy(ImVec2(10.0f, 0.0f));

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
		static int num = 0;
		std::string buttonID = "EDIT";
		buttonID += std::to_string(m_enumButtonIndex);
		if (ImGui::JHButton(&data, "Edit"))
		{
			if (m_enumFlagList[m_enumButtonIndex] == false)
			{
				m_enumFlagList[m_enumButtonIndex] = true;
				for (int i = 0; i < m_enumFlagList.size(); i++)
				{
					if (i != m_enumButtonIndex)
					{
						m_enumFlagList[i] = false;
					}
				}
			}
			else
			{
				m_enumFlagList[m_enumButtonIndex] = false;
			}
		}

		ImGui::PopStyleColor(3);

		if (m_enumFlagList[m_enumButtonIndex] == true)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 0.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

			float contentWidth = 260;
			float contentHeight = 65;

			ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - contentWidth) / 2 + 10);

			ImGui::BeginChild("enumBackground", ImVec2(contentWidth + 20, contentHeight + 20), true);
			ImGui::BeginChild("enumMain", ImVec2(contentWidth, contentHeight), false);

			for (int i = 0; i < symbols.size(); i++)
			{
				if (i != value)
				{
					if (ImGui::Button(symbols[i]))
					{
						m_enumFlagList[m_enumButtonIndex] = false;
						data = static_cast<enumType>(i);
					}
				}
			}
			ImGui::PopStyleColor(3);

			ImGui::EndChild();
			ImGui::EndChild();
		}
	}
	m_enumButtonIndex++;
}

template<>
void InspectorPanel::DrawComponentInfo<Transform>(Transform* instance)
{
	// 이 함수와 아래의 <KimjungIl> 함수는 전임자가 싸 놓은 거대한 똥으로 생각한다.
	//
	DrawCenterColorText("Transform");

	ImGui::Spacing();

	ImGui::NewLine();

	SetTransformUI("Pos X", instance->position.x);
	SetTransformUI("Pos Y", instance->position.y);
	SetTransformUI("Pos Z", instance->position.z);

	DebugManager::GetInstance()->DrawDebugText("Select Object Pos : %f, %f, %f", SelectManager::GetInstance()->GetRecentHierarchy()->GetTransform()->position.x, SelectManager::GetInstance()->GetRecentHierarchy()->GetTransform()->position.y, SelectManager::GetInstance()->GetRecentHierarchy()->GetTransform()->position.z);

	ImGui::NewLine();

	SetTransformUI("Scl X", instance->scale.x);
	SetTransformUI("Scl Y", instance->scale.y);
	SetTransformUI("Scl Z", instance->scale.z);

	DebugManager::GetInstance()->DrawDebugText("Select Object Scale : %f, %f, %f", SelectManager::GetInstance()->GetRecentHierarchy()->GetTransform()->scale.x, SelectManager::GetInstance()->GetRecentHierarchy()->GetTransform()->scale.y, SelectManager::GetInstance()->GetRecentHierarchy()->GetTransform()->scale.z);

	ImGui::NewLine();

	/// select 매니저에서 쿼터니언 값을 오일러 값으로 바꾼 로테이션 값을 들고 있음
	SetTransformUI("Rot X", SelectManager::GetInstance()->GetNowRotation().x);
	SetTransformUI("Rot Y", SelectManager::GetInstance()->GetNowRotation().y);
	SetTransformUI("Rot Z", SelectManager::GetInstance()->GetNowRotation().z);

	Quaternion tempQuart(SelectManager::GetInstance()->GetNowRotation());

	instance->rotation = tempQuart;

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

	if (ImGui::Button("Rotate X + 90"))
	{
		auto eulerRotation = SelectManager::GetInstance()->GetNowRotation();
		eulerRotation.x += 90;
		SelectManager::GetInstance()->GetNowRotation() = eulerRotation;
	}

	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::Dummy(ImVec2(65.0f, 0.0f));
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

	if (ImGui::Button("Rotate Y + 90"))
	{
		auto eulerRotation = SelectManager::GetInstance()->GetNowRotation();
		eulerRotation.y += 90;
		SelectManager::GetInstance()->GetNowRotation() = eulerRotation;
	}

	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::Dummy(ImVec2(65.0f, 0.0f));
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

	if (ImGui::Button("Rotate Z + 90"))
	{
		auto eulerRotation = SelectManager::GetInstance()->GetNowRotation();
		eulerRotation.z += 90;
		SelectManager::GetInstance()->GetNowRotation() = eulerRotation;
	}

	ImGui::PopStyleColor(3);

	DebugManager::GetInstance()->DrawDebugText("Select Object Rotation : %f, %f, %f, %f", instance->rotation.x, SelectManager::GetInstance()->GetRecentHierarchy()->GetTransform()->rotation.y, SelectManager::GetInstance()->GetRecentHierarchy()->GetTransform()->rotation.z, SelectManager::GetInstance()->GetRecentHierarchy()->GetTransform()->rotation.w);
	DrawLine(IM_COL32(255, 255, 255, 255), 3.0f);
}

template<>
void InspectorPanel::DrawComponentInfo<SkinnedMeshWrapper>(SkinnedMeshWrapper* instance)
{
	float cursorPosY = ImGui::GetCursorPosY();

	DrawCenterColorText("SkinnedMesh");

	ImGui::Spacing();
	DrawSpecificData(instance->meshFilePath, "meshFilePath");
	DrawSpecificData(instance->diffuseFilePath, "diffuseFilePath");
	instance->ApplySettings();

	DeleteComponent(instance, cursorPosY);

	DrawLine(IM_COL32(255, 255, 255, 255), 3.0f);
}
template<>
void InspectorPanel::DrawComponentInfo<StaticMeshWrapper>(StaticMeshWrapper* instance)
{
	float cursorPosY = ImGui::GetCursorPosY();

	DrawCenterColorText("StaticMesh");

	CopyStaticMesh(instance);

	ImGui::SameLine();

	CopyStaticMesh(instance, -1.0f);

	ImGui::SameLine();

	CopyStaticMesh(instance, +1.0f);

	ImGui::SameLine();

	CopyStaticMesh(instance, 0.0f, -1.0f);

	ImGui::SameLine();

	CopyStaticMesh(instance, 0.0f, +1.0f);

	SetStaticMesh(instance);

	DeleteComponent(instance, cursorPosY);

	DrawLine(IM_COL32(255, 255, 255, 255), 3.0f);
}

template<>
void InspectorPanel::DrawComponentInfo<HailKimJungIl>(HailKimJungIl* instance)
{
	float cursorPosY = ImGui::GetCursorPosY();

	DrawCenterColorText("TestComponent");

	ImGui::Spacing();

	/// 여기서 일일히 나열하는게 아닌 component들 중 보여질 것들에 대한 리스트를 만들고
	/// 그 리스트에 이름까지 같이 넣어놓고, 리스트를 순회하면서 이 함수를 사용하면 되지않을까?
	DrawSpecificData(instance->lastOrder);

	DrawSpecificData(instance->haveNuclear, "haveNuclear");

	DrawSpecificData(instance->Height, "Height");

	DrawSpecificData(instance->wifeCount, "wifeCount");

	DrawSpecificData(instance->testVector, "testVector");

	// 여기서 enum class의 항목을 모두 넣어줘야하나 
	std::vector<const char*> testSymbol = { "ohhh","yessss","noooo","gooood", "niceeee" };
	DrawEnumClass<HailKimJungIl::eTest>(instance->alphabet, testSymbol, "testSymbol");

	DeleteComponent(instance, cursorPosY);

	DrawLine(IM_COL32(255, 255, 255, 255), 3.0f);
}

template<>
void InspectorPanel::DrawComponentInfo<Stage>(Stage* instance)
{
	float cursorPosY = ImGui::GetCursorPosY();

	DrawCenterColorText("Stage");

	ImGui::Spacing();
	DrawSpecificData(instance->m_StageIndex, "stage index");
	DrawSpecificData(instance->m_goalHeightSize, "Height");
	DrawSpecificData(instance->m_goalWidthSize, "Width");
	DrawSpecificData(instance->m_interval, "Interval");
	//DrawSpecificData(Stage::ProceedInterval, "ProceedInterval");
	DrawSpecificData(instance->arrowInterval, "arrow y interval");
	DrawSpecificData(instance->arrowWidth, "arrow width");

	if (instance->m_goalWidthSize > 0 && instance->m_goalHeightSize == 0)
	{
		instance->m_goalHeightSize = 1;
	}
	else if (instance->m_goalHeightSize > 0 && instance->m_goalWidthSize == 0)
	{
		instance->m_goalWidthSize = 1;
	}

	if (instance->m_goalHeightSize < 0)
	{
		instance->m_goalHeightSize = 0;
	}
	else if (instance->m_goalWidthSize < 0)
	{
		instance->m_goalWidthSize = 0;
	}

	if (instance->m_interval < 0)
	{
		instance->m_interval = 0;
	}
	instance->SetBoard();

	DeleteComponent(instance, cursorPosY);

	DrawLine(IM_COL32(255, 255, 255, 255), 3.0f);
}
template<>
void InspectorPanel::DrawComponentInfo<Tile>(Tile* instance)
{
	float cursorPosY = ImGui::GetCursorPosY();

	DrawCenterColorText("Tile");

	ImGui::Spacing();
	const static vector<const char*> tileTypeSymbols
	{ "None", "Blocked", "Robot", "Battery", "Laser", "Button", "Destination" };
	const static vector<const char*> directionSymbols
	{ "Up", "Right", "Down", "Left" };

	DrawEnumClass(instance->tileType, tileTypeSymbols, "tile type");
	DrawSpecificData(instance->energy, "Energy");
	DrawSpecificData(instance->mechanismIndex, "mechanism Index");
	DrawEnumClass(instance->LaserDirection, directionSymbols, "laser direction");
	instance->ApplyChanges();

	DeleteComponent(instance, cursorPosY);

	DrawLine(IM_COL32(255, 255, 255, 255), 3.0f);
}

template<>
void InspectorPanel::DrawComponentInfo<htkg::IComponentUI>(htkg::IComponentUI* instance)
{
	DrawCenterColorText("TestComponent");

	ImGui::Spacing();

	/// 여기서 일일히 나열하는게 아닌 component들 중 보여질 것들에 대한 리스트를 만들고
	/// 그 리스트에 이름까지 같이 넣어놓고, 리스트를 순회하면서 이 함수를 사용하면 되지않을까?
	// 라는	발상으로 만든 것이 IComponentUI이다.
	// 그래서 IComponent의 리스트를 사용해 여차저차하면....


	/*
	instance->........

	DrawSpecificData();
	*/

	DrawLine(IM_COL32(255, 255, 255, 255), 3.0f);
}

template<>
void InspectorPanel::DrawComponentInfo<yunutyEngine::graphics::UIImage>(yunutyEngine::graphics::UIImage* instance)
{
	std::string changeImage;
	std::string texturestring;

	if (m_imageListPopUp)
	{
		changeImage = "Close PopUp";
	}
	else
	{
		changeImage = "Change Image";
	}

	float cursorPosY = ImGui::GetCursorPosY();

	DrawCenterColorText("UI Image");

	ImGui::Spacing();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

	if (imageList.find(instance->GetGameObject()) != imageList.end())
	{
		ImGui::Text("Texture : %s", imageList.at(instance->GetGameObject()).c_str());
	}

	ImGui::SameLine();

	if (ImGui::Button(changeImage.c_str()))
	{
		if (m_imageListPopUp == true)
		{
			m_imageListPopUp = false;
		}
		else
		{
			m_imageListPopUp = true;
		}
	}

	ImGui::PopStyleColor(3);

	if (m_imageListPopUp == true)
	{
		texturestring = GetFileNameByKey(instance->GetGameObject());

		auto iter = imageList.find(instance->GetGameObject());
		if (iter != imageList.end() && texturestring.empty() == false)
		{
			iter->second = texturestring;
		}
		else
		{
			if (texturestring.empty() == false)
			{
				imageList.insert(std::make_pair(instance->GetGameObject(), texturestring.c_str()));
			}
		}
	}

	if (imageList.find(instance->GetGameObject()) != imageList.end())
	{
		instance->GetGI().SetImage(imageList.at(instance->GetGameObject()).c_str());
	}

	auto nowObject = SelectManager::GetInstance()->GetRecentHierarchy();
	DrawSpecificData(nowObject->GetTransform()->position, "Position");
	if (nowObject->GetTransform()->position.z < 0)
	{
		nowObject->GetTransform()->position.z = 0;
	}

	DrawSpecificData(nowObject->GetTransform()->scale, "Scale");
	if (nowObject->GetTransform()->position.z < 0)
	{
		nowObject->GetTransform()->scale.z = 0;
	}
	// UI 이미지 정보 텍스트로 출력하고 이를 변경할 수 있게 해주면 좋을 듯

	DeleteComponent(instance, cursorPosY);

	DrawLine(IM_COL32(255, 255, 255, 255), 3.0f);
}

template<>
void InspectorPanel::DrawComponentInfo<yunutyEngine::graphics::UIText>(yunutyEngine::graphics::UIText* instance)
{
	float cursorPosY = ImGui::GetCursorPosY();
	
	DrawCenterColorText("UI Text");
	
	ImGui::Spacing();
	
	std::string textString;
	
	if (m_UITextPopUp)
	{
		textString = "Close PopUp";
	}
	else
	{
		textString = "Change Text";
	}
	
	ImGui::Spacing();
	
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

	ImGui::Text("Text : %s", instance->GetGI().GetText().c_str());

	ImGui::PopStyleColor(3);
	
	DeleteComponent(instance, cursorPosY);
	
	DrawLine(IM_COL32(255, 255, 255, 255), 3.0f);
}

template<>
void InspectorPanel::DrawComponentInfo<yunutyEngine::graphics::EventHandler>(yunutyEngine::graphics::EventHandler* instance)
{
	float cursorPosY = ImGui::GetCursorPosY();

	DrawCenterColorText("Event Handler");

	ImGui::Spacing();

	auto iter = buttonTypeList.find(instance->GetGameObject());
	if (iter != buttonTypeList.end())
	{
		const static vector<const char*> buttonTypeSymbols
		{ "None", "GameStart", "Credit", "GameQuit", "Play", "Stage", "Back", "Run", "Menu", "Continue", "Restart", "MenuQuit", "NextStage", "StageClear" };

		DrawEnumClass(buttonTypeList.at(instance->GetGameObject()), buttonTypeSymbols, "Button Type");
	}
	else
	{
		ImGui::Text("Button Type : Default");
	}

	DeleteComponent(instance, cursorPosY);

	DrawLine(IM_COL32(255, 255, 255, 255), 3.0f);
}

template<>
void InspectorPanel::DrawComponentInfo<Component>(Component* instance)
{
	// 여기는 개 못짠 코드인데, 기존에 쓰이는 레거시 코드를 살리면서 
	// 리펙터링 해야 함.
	// 현업에서는 함부로 기존 코드를 수정했다가 ㅈ되는 일이 잦기 때문
	if (auto casted = dynamic_cast<Transform*>(instance); casted != nullptr)
	{
		DrawComponentInfo(casted);
	}
	else if (auto casted = dynamic_cast<HailKimJungIl*>(instance); casted != nullptr)
	{
		DrawComponentInfo(casted);
	}
	else if (auto casted = dynamic_cast<htkg::IComponentUI*>(instance); casted != nullptr)
	{
		DrawComponentInfo(casted);
	}
	else if (auto casted = dynamic_cast<Stage*>(instance); casted != nullptr)
	{
		DrawComponentInfo(casted);
	}
	else if (auto casted = dynamic_cast<Tile*>(instance); casted != nullptr)
	{
		DrawComponentInfo(casted);
	}
	else if (auto casted = dynamic_cast<SkinnedMeshWrapper*>(instance); casted != nullptr)
	{
		DrawComponentInfo(casted);
	}
	else if (auto casted = dynamic_cast<StaticMeshWrapper*>(instance); casted != nullptr)
	{
		DrawComponentInfo(casted);
	}
	else if (auto casted = dynamic_cast<yunutyEngine::graphics::UIImage*>(instance); casted != nullptr)
	{
		DrawComponentInfo(casted);;
	}
	else if (auto casted = dynamic_cast<yunutyEngine::graphics::UIText*>(instance); casted != nullptr)
	{
		DrawComponentInfo(casted);;
	}
	else if (auto casted = dynamic_cast<yunutyEngine::graphics::EventHandler*>(instance); casted != nullptr)
	{
		DrawComponentInfo(casted);;
	}
}

void InspectorPanel::DrawNowObjectInfo()
{
	auto nowObject = SelectManager::GetInstance()->GetRecentHierarchy();

	if (nowObject != nullptr)
	{
		DrawCenterColorText(nowObject->getName(), ImVec4(1.0f, 0.0f, 1.0f, 1.0f));

		DrawIsActive(nowObject);

		DrawLine(IM_COL32(255, 255, 255, 255), 3.0f);

		for (auto eachComponent : nowObject->GetIndexedComponents())
		{
			DrawComponentInfo(eachComponent);
		}

		DrawAddComponentButton();

		DrawComponentList();
	}
}

void InspectorPanel::SetTransformUI(std::string type, double& value)
{
	ImGui::SameLine();
	ImGui::Text(type.c_str());
	ImGui::SameLine();
	ImGui::SetNextItemWidth(85);
	std::string a = "##";
	std::string result = a + type;
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
	ImGui::InputDouble(result.c_str(), &value, 0.001f, 0.0f, "%.3f");

	ImGui::PopStyleColor(5);
}

void InspectorPanel::SetDebugFlag()
{
	if (ImGui::IsKeyDown(ImGuiKey_PageUp) && m_DebugFlag == false)
	{
		m_DebugFlag = true;
		DebugManager::GetInstance()->SetDebugMode(true);
	}
	else if (ImGui::IsKeyDown(ImGuiKey_PageDown) && m_DebugFlag == true)
	{
		m_DebugFlag = false;
		DebugManager::GetInstance()->SetDebugMode(false);
	}
}

void InspectorPanel::DrawLine(ImU32 lineColor, float thickness)
{
	ImGui::Dummy(ImVec2(0, 2.0f));

	ImDrawList* drawList = ImGui::GetWindowDrawList();

	ImVec2 startPoint(ImGui::GetCursorScreenPos().x - 10, ImGui::GetCursorScreenPos().y);
	float contentWidth = ImGui::GetContentRegionAvail().x;
	ImVec2 endPoint(ImGui::GetCursorScreenPos().x + contentWidth + 2 * ImGui::GetCursorPos().x, ImGui::GetCursorScreenPos().y);

	drawList->AddLine(startPoint, endPoint, lineColor, thickness);

	ImGui::Dummy(ImVec2(0, 2.0f));
}

void InspectorPanel::DrawAddComponentButton()
{
	std::string addComponent = "Add Component";
	ImGui::NewLine();
	ImVec2 textSize = ImGui::CalcTextSize(addComponent.c_str());
	ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - textSize.x) / 2);

	if (isAddComponetClick == false)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
	}
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

	if (ImGui::Button("Add Component"))
	{
		if (isAddComponetClick == false)
		{
			isAddComponetClick = true;
		}
		else
		{
			isAddComponetClick = false;
		}
	}

	ImGui::PopStyleColor(3);
}

void InspectorPanel::DrawCenterColorText(std::string text, ImVec4 color)
{
	ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
	ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - textSize.x) / 2);

	ImGui::TextColored(color, text.c_str());
}

void InspectorPanel::DrawComponentList()
{
	/// transform 제외 component들을 나열해야하는데...
	/// component를 상속받는 객체들을 리스트에 넣어놓고 불러오고싶은데 어떻게할까


	if (isAddComponetClick == true)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

		float contentWidth = 260;
		float contentHeight = 65;

		ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - contentWidth) / 2);

		ImGui::BeginChild("scrollBackground", ImVec2(contentWidth + 20, contentHeight + 20), true);
		ImGui::BeginChild("scrollMain", ImVec2(contentWidth, contentHeight), false);

		if (ImGui::Button("Stage"))
		{
			if (SelectManager::GetInstance()->GetRecentHierarchy()->GetComponent<Stage>() == nullptr)
			{
				SelectManager::GetInstance()->GetRecentHierarchy()->AddComponent<Stage>();
			}
			isAddComponetClick = false;
		}

		else if (ImGui::Button("test component"))
		{
			//if (SelectManager::GetInstance()->GetRecentHierarchy()->GetComponent<HailKimJungIl>() == nullptr)
			{
				SelectManager::GetInstance()->GetRecentHierarchy()->AddComponent<HailKimJungIl>();
			}
			isAddComponetClick = false;
		}
		else if (ImGui::Button("UI Image"))
		{
			if (SelectManager::GetInstance()->GetRecentHierarchy()->GetComponent<yunutyEngine::graphics::UIImage>() == nullptr)
			{
				auto comp = SelectManager::GetInstance()->GetRecentHierarchy()->AddComponent<yunutyEngine::graphics::UIImage>();

				//// 기본으로 여우, string 값 받아 변경할 예정 -> 리소스 매니저에서 텍스쳐 이름을 키값으로 가져와야하는데
				// 정해진 것만 가져올 수 있는 듯
				comp->GetGI().SetImage("");

				comp->GetTransform()->SetWorldPosition(Vector3d(0.0f, 0.0f, 0.0f));
				comp->GetTransform()->scale = Vector3d(0.1f, 0.1f, 0.0f);
			}

			isAddComponetClick = false;
		}

		else if (ImGui::Button("UI Text"))
		{
			if (SelectManager::GetInstance()->GetRecentHierarchy()->GetComponent<yunutyEngine::graphics::UIText>() == nullptr)
			{
 				auto comp = SelectManager::GetInstance()->GetRecentHierarchy()->AddComponent<yunutyEngine::graphics::UIText>();
 				comp->GetGI().SetText((TCHAR*)L"NULL");
 				comp->GetGI().SetWorldSpace();
 
 				comp->GetTransform()->SetWorldPosition(Vector3d(0.0f, 0.0f, 0.0f));
 				comp->GetTransform()->scale = Vector3d(0.1f, 0.1f, 0.0f);
			}

			isAddComponetClick = false;
		}

		else if (ImGui::Button("Button Logic"))
		{
			if (SelectManager::GetInstance()->GetRecentHierarchy()->GetComponent<yunutyEngine::graphics::EventHandler>() == nullptr)
			{
				auto uiEventHandler = SelectManager::GetInstance()->GetRecentHierarchy()->AddComponent<yunutyEngine::graphics::EventHandler>();
				//m_buttonLogic.insert(std::make_pair(SelectManager::GetInstance()->GetRecentHierarchy(), nullptr));

				buttonTypeList.insert(std::make_pair(SelectManager::GetInstance()->GetRecentHierarchy(), ButtonLogicManager::eButtonType::None));

				uiEventHandler->SetClickCallback([=]()
					{
						if (buttonTypeList.at(SelectManager::GetInstance()->GetRecentHierarchy()) != ButtonLogicManager::eButtonType::None)
						{
							//using FunctionType = void (*)()
							//FunctionType func = reinterpret_cast<void(*)()>(buttonLogic);
							//func();

							auto buttonlogic = m_pButtonLogicManager->GetLogicMap().at(buttonTypeList.at(SelectManager::GetInstance()->GetRecentHierarchy()));
							buttonlogic();
						}
						DebugPanel::LogMessage((uiEventHandler->GetGameObject()->getName() + " clicked").c_str());
					});
			}

			isAddComponetClick = false;
		}

		//else if (ImGui::Button("Skinned Mesh"))
		//{
		//	if (SelectManager::GetInstance()->GetRecentHierarchy()->GetComponent<SkinnedMeshWrapper>() == nullptr)
		//	{
		//		SelectManager::GetInstance()->GetRecentHierarchy()->AddComponent<SkinnedMeshWrapper>();
		//	}
		//	isAddComponetClick = false;
		//}

		else if (ImGui::Button("Static Mesh"))
		{
			if (SelectManager::GetInstance()->GetRecentHierarchy()->GetComponent<StaticMeshWrapper>() == nullptr)
			{
				SelectManager::GetInstance()->GetRecentHierarchy()->AddComponent<StaticMeshWrapper>();
			}
			isAddComponetClick = false;
		}

		else if (ImGui::Button("Tile"))
		{
			if (SelectManager::GetInstance()->GetRecentHierarchy()->GetComponent<Tile>() == nullptr)
			{
				SelectManager::GetInstance()->GetRecentHierarchy()->AddComponent<Tile>();
			}
			isAddComponetClick = false;
		}

		ImGui::PopStyleColor(3);

		ImGui::EndChild();
		ImGui::EndChild();
	}
}

template<typename ComponentType>
void InspectorPanel::DeleteComponent(ComponentType* instance, float cursorY)
{
	float originCursorY = ImGui::GetCursorPos().y;

	ImGui::SameLine();
	std::string compChildObjectName;

	// 컴포넌트로 인해 자식 객체가 생길때 자식 객체의 이름에 포함되는 문자열을 넣기
	if constexpr (std::is_same_v<ComponentType, Stage>)
	{
		compChildObjectName = "board";
	}
	else if constexpr (std::is_same_v<ComponentType, HailKimJungIl>)
	{
		compChildObjectName = "Test";
	}
	else if constexpr (std::is_same_v<ComponentType, yunutyEngine::graphics::UIImage>)
	{
		compChildObjectName = "UIImage";
	}
	else if constexpr (std::is_same_v<ComponentType, yunutyEngine::graphics::UIText>)
	{
		compChildObjectName = "UIText";
	}
	else if constexpr (std::is_same_v<ComponentType, yunutyEngine::graphics::EventHandler>)
	{
		compChildObjectName = "EventHandler";
	}
	else if constexpr (std::is_same_v<ComponentType, SkinnedMeshWrapper>)
	{
		compChildObjectName = "SkinnedMeshWrapper";
	}
	else if constexpr (std::is_same_v<ComponentType, StaticMeshWrapper>)
	{
		compChildObjectName = "StaticMeshWrapper";
	}
	else if constexpr (std::is_same_v<ComponentType, Tile>)
	{
		compChildObjectName = "Tile";
	}

	if (compChildObjectName.empty() == false)
	{
		float textSize = 115.0f;
		ImGui::SetCursorPosX((ImGui::GetContentRegionMax().x) - textSize);
		ImGui::SetCursorPosY(cursorY);

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));


		if (ImGui::JHButton(instance, "Delete Component"))
		{
			/// stage를 지울 경우에 stage component를 만들때 자식 객체에 생긴 모든 오브젝트를 지워버려야함
			if (instance->GetGameObject()->GetChildren().size() > 0)
			{
				for (auto child : instance->GetGameObject()->GetChildren())
				{
					if (child->getName().find(compChildObjectName) != std::string::npos)
					{
						//SelectManager::GetInstance()->DeleteHierarchyList(child);
						Scene::getCurrentScene()->DestroyGameObject(child);
					}
				}
			}

			DeleteMapInComPonent(instance);

			SelectManager::GetInstance()->GetRecentHierarchy()->DeleteComponent(instance);
		}

		ImGui::PopStyleColor(3);
	}

	ImGui::SetCursorPosY(originCursorY);
}

void InspectorPanel::DeleteMapInComPonent(yunutyEngine::Component* instance)
{
	if (auto imageInstance = dynamic_cast<yunutyEngine::graphics::UIImage*>(instance))
	{
		auto imageIter = imageList.find(instance->GetGameObject());
		if (imageIter != imageList.end())
		{
			imageList.erase(instance->GetGameObject());
		}

	}
	else if (auto buttonInstance = dynamic_cast<yunutyEngine::graphics::EventHandler*>(instance))
	{
		auto buttonIter = buttonTypeList.find(instance->GetGameObject());
		if (buttonIter != buttonTypeList.end())
		{
			buttonTypeList.erase(instance->GetGameObject());
		}
	}
}


const std::string InspectorPanel::GetFileNameInFolder(std::string buttonName)
{
	static ImGui::FileBrowser fileDialog;
	static ImVec2 dialogPosition;

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

	if (ImGui::Button(buttonName.c_str()))
	{
		dialogPosition = ImGui::GetMousePos();
		fileDialog.Open();
	}

	ImGui::PopStyleColor(3);

	ImGui::SetNextWindowPos(dialogPosition);

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

	fileDialog.Display();

	ImGui::PopStyleColor(3);

	if (fileDialog.HasSelected())
	{
		std::string selectedFileName = fileDialog.GetSelected().string();
		fileDialog.ClearSelected();

		return selectedFileName;
	}
	else
	{
		std::string temp;
		return temp;
	}
}

const std::string InspectorPanel::GetFileNameByKey(yunutyEngine::GameObject* object)
{
	std::string textureKey;

	auto textureKeys = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager()->GetTextureKeys();

	ImGui::BeginChild("FileSearch", ImVec2(0, 200), true, ImGuiWindowFlags_HorizontalScrollbar);

	for (auto key : textureKeys)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

		if (imageList.find(object) != imageList.end())
		{
			if (imageList.at(object) == key)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
			}
		}

		if (ImGui::Button(key.c_str()))
		{
			textureKey = key;
			m_imageListPopUp = false;
		}

		if (imageList.find(object) != imageList.end())
		{
			if (imageList.at(object) == key)
			{
				ImGui::PopStyleColor(4);
			}
			else
			{
				ImGui::PopStyleColor(3);
			}
		}
		else
		{
			ImGui::PopStyleColor(3);
		}
	}

	ImGui::EndChild();

	return textureKey;
}

void InspectorPanel::DrawIsActive(yunutyEngine::GameObject* nowObject)
{
	auto iter = isAcitvateList.find(nowObject);
	if (iter != isAcitvateList.end())
	{
		iter->second = nowObject->GetSelfActive();
	}
	else
	{
		isAcitvateList.insert(std::make_pair(nowObject, nowObject->GetSelfActive()));
	}
	DrawSpecificData(isAcitvateList.at(nowObject), "Activate");
	nowObject->SetSelfActive(isAcitvateList.at(nowObject));
}

void InspectorPanel::SetStaticMesh(StaticMeshWrapper* instance)
{
	// Mesh
	std::string changeMesh;
	if (m_meshListPopUp)
	{
		changeMesh = "Close PopUp";
	}
	else
	{
		changeMesh = "Change Mesh";
	}

	ImGui::Spacing();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

	ImGui::Text("Mesh : %s", instance->meshFilePath.c_str());

	ImGui::SameLine();
	if (ImGui::Button(changeMesh.c_str()))
	{
		if (m_meshListPopUp == true)
		{
			m_meshListPopUp = false;
		}
		else
		{
			m_meshListPopUp = true;
		}
	}

	ImGui::PopStyleColor(3);

	if (m_meshListPopUp == true)
	{
		auto meshKeys = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager()->GetMeshKeys();
		ImGui::BeginChild("StaticMeshSearch", ImVec2(0, 200), true, ImGuiWindowFlags_HorizontalScrollbar);
		for (auto key : meshKeys)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

			if (ImGui::Button(key.c_str()))
			{
				instance->meshFilePath = key;
				m_meshListPopUp = false;
			}

			ImGui::PopStyleColor(3);
		}

		ImGui::EndChild();
	}

	// Diffuse
	std::string changeDiffuse;
	if (m_DiffuseListPopUp)
	{
		changeDiffuse = "Close PopUp";
	}
	else
	{
		changeDiffuse = "Change Diffuse";
	}

	ImGui::Spacing();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

	ImGui::Text("Diffuse : %s", instance->diffuseFilePath.c_str());

	ImGui::SameLine();
	if (ImGui::Button(changeDiffuse.c_str()))
	{
		if (m_DiffuseListPopUp == true)
		{
			m_DiffuseListPopUp = false;
		}
		else
		{
			m_DiffuseListPopUp = true;
		}
	}

	ImGui::PopStyleColor(3);

	if (m_DiffuseListPopUp == true)
	{
		auto diffuseKeys = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager()->GetTextureKeys();
		ImGui::BeginChild("DiffuseSearch", ImVec2(0, 200), true, ImGuiWindowFlags_HorizontalScrollbar);
		for (auto key : diffuseKeys)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

			if (ImGui::Button(key.c_str()))
			{
				instance->diffuseFilePath = key;
				m_DiffuseListPopUp = false;
			}

			ImGui::PopStyleColor(3);
		}

		ImGui::EndChild();
	}

	// Normal
	std::string NormalDiffuse;
	if (m_NormalListPopUp)
	{
		NormalDiffuse = "Close PopUp";
	}
	else
	{
		NormalDiffuse = "Change Normal";
	}

	ImGui::Spacing();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

	ImGui::Text("Normal : %s", instance->normalFilePath.c_str());

	ImGui::SameLine();
	if (ImGui::Button(NormalDiffuse.c_str()))
	{
		if (m_NormalListPopUp == true)
		{
			m_NormalListPopUp = false;
		}
		else
		{
			m_NormalListPopUp = true;
		}
	}

	ImGui::PopStyleColor(3);

	if (m_NormalListPopUp == true)
	{
		auto normalKeys = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager()->GetTextureKeys();
		ImGui::BeginChild("DiffuseSearch", ImVec2(0, 200), true, ImGuiWindowFlags_HorizontalScrollbar);
		for (auto key : normalKeys)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

			if (ImGui::Button(key.c_str()))
			{
				instance->normalFilePath = key;
				m_NormalListPopUp = false;
			}

			ImGui::PopStyleColor(3);
		}

		ImGui::EndChild();
	}

	if (instance->meshFilePath.empty() == false && instance->diffuseFilePath.empty() == false && instance->normalFilePath.empty() == false)
	{
		instance->ApplySettings();
	}
}

void InspectorPanel::CopyStaticMesh(StaticMeshWrapper* instance, float xinterval, float zInterval)
{
	std::string buttonName;
	if (xinterval > 0)
	{
		buttonName = "Copy +Right";
	}
	else if (xinterval < 0)
	{
		buttonName = "Copy -Right";
	}
	else if (xinterval == 0 && zInterval > 0)
	{
		buttonName = "Copy +Up";
	}
	else if (xinterval == 0 && zInterval < 0)
	{
		buttonName = "Copy -Up";
	}
	else
	{
		buttonName = "Copy";
	}

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

	if (ImGui::Button(buttonName.c_str()))
	{
		auto object = instance->GetGameObject();
		auto newObject = Scene::getCurrentScene()->AddGameObject();
		//std::string newObjectName = newObject->getName() + "*";
		//newObject->setName(newObjectName.c_str());

		if (object->GetParentGameObject() != nullptr)
		{
			newObject->SetParent(object->GetParentGameObject());
		}

		newObject->GetTransform()->position.x = object->GetTransform()->position.x + xinterval;
		newObject->GetTransform()->position.y = object->GetTransform()->position.y;
		newObject->GetTransform()->position.z = object->GetTransform()->position.z + zInterval;

		newObject->GetTransform()->rotation = object->GetTransform()->rotation;
		newObject->GetTransform()->scale = object->GetTransform()->scale;

		auto comp = object->GetComponent<StaticMeshWrapper>();
		if (comp != nullptr)
		{
			auto newObjectComp = newObject->AddComponent<StaticMeshWrapper>();
			newObjectComp->meshFilePath = comp->meshFilePath;
			newObjectComp->diffuseFilePath = comp->diffuseFilePath;
			newObjectComp->normalFilePath = comp->normalFilePath;
			newObjectComp->ApplySettings();
		}
		SelectManager::GetInstance()->SetRecentHierarchy(newObject);
	}
	ImGui::PopStyleColor(3);
}


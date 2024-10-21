/// 씬의 정보들을 계층 구조에 따라서 목록화한 패널
/// 컴포넌트를 추가하거나 삭제가 가능
/// 추가한 컴포넌트에 대한 수정이 가능
/// 2023. 08. 23. 윤종화

#pragma once
#include <string>
#include <functional>
#include "ButtonLogicManager.h"
#include "YunutyEngine.h"
#include "imgui.h"
import ui.EditorPanel;
import htkg.InGame.StaticMeshWrapper;
export module ui.InspectorPanel;

export class InspectorPanel
	: public EditorPanel
{
public:
	virtual void Init() override;
	virtual void Shutdown() override;
	virtual void OnUpdate(float ts) override;
	virtual void DrawPanel() override;

	void DrawNowObjectInfo();

	template<typename enumType>
	//const char* enumSymbols[]
	//typename std::enable_if<std::is_enum<enumType>::value>::type
	void DrawEnumClass(enumType& data, const vector<const char*>& symbols, const std::string name);

	template<typename ComponentType>
	void DrawComponentInfo(ComponentType* instance);

	void SetTransformUI(std::string type, double& value);

	void DrawAddComponentButton();

	void DrawComponentList();

	void DrawIsActive(yunutyEngine::GameObject* nowObject);

private:
	void SetDebugFlag();
	void DrawLine(ImU32 lineColor, float thickness);
	void DrawCenterColorText(std::string text, ImVec4 color = ImVec4(0.0f, 1.0f, 1.0f, 1.0f));
	
	// 출력하려는 Data가 문자열이 아닌 경우에는 매개변수에 name 값을 넣어줄 것
	template<typename DataType>
	void DrawSpecificData(DataType& data, const std::string name = std::string());

	template<typename ComponentType>
	void DeleteComponent(ComponentType* instance, float cursorY);

	void DeleteMapInComPonent(yunutyEngine::Component* instance);

	const std::string GetFileNameInFolder(std::string buttonName);

	const std::string GetFileNameByKey(yunutyEngine::GameObject* object);

	void SetStaticMesh(StaticMeshWrapper* instance);

	void CopyStaticMesh(StaticMeshWrapper* instance, float xinterval=0.0f, float zInterval=0.0f);

public:
	static bool isAddComponetClick;
	static std::unordered_map<yunutyEngine::GameObject*, bool> isAcitvateList;
	static std::unordered_map<yunutyEngine::GameObject*, ButtonLogicManager::eButtonType> buttonTypeList;
	static std::unordered_map<yunutyEngine::GameObject*, std::string> imageList;
	static std::unordered_map<yunutyEngine::GameObject*, std::string> stringList;

private:
	bool m_DebugFlag = false;

	std::vector<bool> m_enumFlagList;
	unsigned int m_enumButtonIndex = 0;

	/// 어떤 식으로 할까 고민중
	std::vector<Component> m_ComponentList;

	char inputStringBuffer[256];

	ButtonLogicManager* m_pButtonLogicManager = nullptr;

	bool m_imageListPopUp = false;

	bool m_meshListPopUp = false;

	bool m_DiffuseListPopUp = false;

	bool m_NormalListPopUp = false;

	bool m_UITextPopUp = false;

};

// 아래의 구현은 먼 미래에 고민해볼 영역으로 두자
//template<typename ComponentType>
//void DrawComponentInfo(ComponentType* instance)
//{
//	// 기상천외한 방법
//}
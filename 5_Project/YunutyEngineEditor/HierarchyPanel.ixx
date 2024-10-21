/// 씬의 정보들을 계층 구조에 따라서 목록화한 패널
/// 오브젝트 추가, 삭제, 오브젝트 이름에 대한 수정이 가능
/// 마우스 드래그를 통해 현재 씬 내의 오브젝트에 대한 계층 구조를 바꾸는 것이 가능
/// 2023. 08. 23. 윤종화

module;
#include <vector>
#include "YunutyEngine.h"
#include "imgui_internal.h"
export import ui.EditorPanel;
export module ui.HierarchyPanel;

export class HierarchyPanel
	: public EditorPanel
{
public:
	virtual void Init() override;
	virtual void Shutdown() override;
	virtual void OnUpdate(float ts) override;
	virtual void DrawPanel() override;

private:
	void CreateOneTreeNode(yunutyEngine::GameObject* root);
	
	void RightClickPopUp(GameObject* obj);

	void ClickLogic(GameObject* obj);

	void LeftClickSelected(GameObject* obj);

	void DragLogic(GameObject* obj);

	void TextNearMouse(std::string text);

	void RightClick();

	void CalculateBlank();

	void DeleteObject(GameObject* obj);

private:
	int m_NamingNum = 0;

	bool m_popUpFlag = false;

	bool m_setNameFlag = false;

	bool m_rightFlag = false;

	bool m_isDragging = false;

	yunutyEngine::GameObject* m_dragTarget;

	float m_prevYcursor = 0.0f;

	std::vector<yunutyEngine::GameObject*> m_ObjectList;

	std::vector<int> m_IndexList;

	char inputStringBuffer[256];
};

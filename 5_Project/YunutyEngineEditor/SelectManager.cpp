#include "imgui.h"
#include "imgui_internal.h"
#include "YunutyEngine.h"
#include "SelectManager.h"
import ui.InspectorPanel;

SelectManager* SelectManager::instance = nullptr;

SelectManager::SelectManager()
{

}

SelectManager::~SelectManager()
{

}

SelectManager* SelectManager::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new SelectManager();
	}
	return instance;
}

void SelectManager::Delete()
{
	delete instance;
}

yunutyEngine::GameObject*& SelectManager::GetRecentHierarchy()
{
	return m_recentHierarchy;
}

void SelectManager::SetRecentHierarchy(yunutyEngine::GameObject* obj)
{
	m_recentHierarchy = obj;
	InspectorPanel::isAddComponetClick = false;
	if (obj != nullptr)
	{
		SetNowRotation(obj);
	}
}

const std::set<yunutyEngine::GameObject*>& SelectManager::GetHierarchyListSet()
{
	return m_HierarchyListSet;
}

void SelectManager::InsertHierarchyList(yunutyEngine::GameObject* obj)
{
	m_HierarchyListSet.insert(obj);
}

void SelectManager::DeleteHierarchyList(yunutyEngine::GameObject* obj)
{
	m_HierarchyListSet.erase(obj);

	for (auto child : obj->GetChildren())
	{
		m_HierarchyListSet.erase(child);
	}
}

void SelectManager::InitializeHierarchy()
{
	set<yunutyEngine::GameObject*> tempSet;
	m_HierarchyListSet.swap(tempSet);

	m_recentHierarchy = nullptr;
}

yunutyEngine::Vector3d& SelectManager::GetNowRotation()
{
	return m_nowRotation;
}

void SelectManager::SetNowRotation(yunutyEngine::GameObject* obj)
{
	m_nowRotation = obj->GetTransform()->rotation.Euler();
}

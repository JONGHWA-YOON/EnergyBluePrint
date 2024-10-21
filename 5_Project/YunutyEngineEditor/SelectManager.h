#pragma once
#include "YunutyEngine.h"

/// 현재 선택된 오브젝트가 무엇인지를 관리하기 위한 클래스
/// 어디서든 현재 선택된 오브젝트가 무엇인지를 접근하기 위해 싱글턴으로 구현
/// 2023.08.23 윤종화

class SelectManager
{
private:
	SelectManager();
	~SelectManager();

	static SelectManager* instance;


public:
	static SelectManager* GetInstance();
	static void Delete();

public:
	yunutyEngine::GameObject*& GetRecentHierarchy();
	void SetRecentHierarchy(yunutyEngine::GameObject* obj);

	const set<yunutyEngine::GameObject*>& GetHierarchyListSet();
	void InsertHierarchyList(yunutyEngine::GameObject* obj);
	void DeleteHierarchyList(yunutyEngine::GameObject* obj);

	void InitializeHierarchy();

	yunutyEngine::Vector3d& GetNowRotation();

	void SetNowRotation(yunutyEngine::GameObject* obj);

private:
	set<yunutyEngine::GameObject*> m_HierarchyListSet;
	yunutyEngine::GameObject* m_recentHierarchy;

	yunutyEngine::Vector3d m_nowRotation;
};



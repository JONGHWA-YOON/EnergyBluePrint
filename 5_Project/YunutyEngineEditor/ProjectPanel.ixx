/// 2023. 07. 09. 김상준
/// 씬의 정보들을 계층 구조에 따라서 목록화한 패널

import ui.EditorPanel;
export module ui.ProjectPanel;

export class ProjectPanel
	: public EditorPanel
{
public:
	virtual void Init() override;
	virtual void Shutdown() override;
	virtual void OnUpdate(float ts) override;
	virtual void DrawPanel() override;
};
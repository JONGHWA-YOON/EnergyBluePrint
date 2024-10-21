//#include "EditorPanel_All.h"
//#include "EditorPanel.h"
module ui.EditorPanel;
import ui;
import ui.HierarchyPanel;
import ui.DebugPanel;

EditorPanel* EditorPanel::CreatePanel(Panel_List panel)
{
	switch (panel)
	{
	case Panel_List::HIERARCHY:
		return new HierarchyPanel();
	case Panel_List::INSPECTOR:
		return new InspectorPanel();
	case Panel_List::PROJECT:
		return new ProjectPanel();
	case Panel_List::SCENE:
		return new ScenePanel();
	case Panel_List::GAMEVIEW:
		return new GameViewPanel();
	case Panel_List::DEBUG:
		return new DebugPanel();
	default:
		return nullptr;
	}
}

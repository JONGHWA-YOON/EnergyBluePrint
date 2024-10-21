module;
#include "Walnut/Layer.h"
#include "imgui.h"
#include <vector>
import ui.EditorPanel;
import graphics.Camera;
export module ui.EditorLayer;

using namespace graphics;

export constexpr unsigned int CAMERA_LIST_INTERVAL{ 100 };

export enum class Camera_List
	: unsigned char
{
	COUNT = 2,
	EDITOR = 0,
	GAMEVIEW = 1 * CAMERA_LIST_INTERVAL
};

export class EditorLayer
	: public Walnut::Layer
{
public:
	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(float ts) override;
	virtual void OnUIRender() override;

	Camera* GetEditorCamera() { return cameraList[(size_t)Camera_List::EDITOR / CAMERA_LIST_INTERVAL]; }
	Camera* GetGameViewCamera() { return cameraList[(size_t)Camera_List::GAMEVIEW / CAMERA_LIST_INTERVAL]; }

	std::vector<Camera*> cameraList;
	std::vector<EditorPanel*> panelList;
private:
};
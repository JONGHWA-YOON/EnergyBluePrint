#include "YunutyEngine.h"
#include "Camera.h"
#include "_YunuGIObjects.h"
using namespace yunutyEngine::graphics;

Camera* yunutyEngine::graphics::Camera::mainCamera = nullptr;
yunutyEngine::graphics::Camera::Camera()
{
    if (mainCamera == nullptr)
        mainCamera = this;
    yunuGICamera = _YunuGIObjects::SingleInstance().factory->CreateCamera({});
}
void yunutyEngine::graphics::Camera::Update()
{
    GetTransform()->scale = Vector3d::one;
    yunuGICamera->SetWorldTM(GetTransform()->GetWorldTM());
}
void yunutyEngine::graphics::Camera::SetCameraMain()
{
    mainCamera = this;
    yunuGICamera->SetAsMain();
}
Camera* yunutyEngine::graphics::Camera::GetMainCamera()
{
    return mainCamera;
}

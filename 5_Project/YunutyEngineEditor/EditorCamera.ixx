module;
#include "YunutyEngine.h"
#include "ButtonLogicManager.h"
export module htkg.EditorCamera;
import htkg.SaveLoad;
import htkg.InGame.StaticMeshWrapper;
import htkg.ButtonLogicManager;

using namespace yunutyEngine;

export namespace htkg
{
    class EditorCamera : public yunutyEngine::graphics::Camera
    {
    protected:
        void Update()
        {
            float cameraSpeed = 10.0f;
            Camera::Update();
            Vector3d deltaPosition = Vector3d::zero;

            if (yunutyEngine::Input::isKeyDown(KeyCode::LShift))
                cameraSpeed = 20.0f;

#ifdef DebugMode
            if (yunutyEngine::Input::isKeyDown(KeyCode::A))
                deltaPosition -= GetTransform()->GetWorldRotation().Right();
            if (yunutyEngine::Input::isKeyDown(KeyCode::D))
                deltaPosition += GetTransform()->GetWorldRotation().Right();
            if (yunutyEngine::Input::isKeyDown(KeyCode::W))
                deltaPosition += GetTransform()->GetWorldRotation().Forward();
            if (yunutyEngine::Input::isKeyDown(KeyCode::S))
                deltaPosition -= GetTransform()->GetWorldRotation().Forward();
            if (yunutyEngine::Input::isKeyDown(KeyCode::Q))
                deltaPosition -= Vector3d::up;
            if (yunutyEngine::Input::isKeyDown(KeyCode::E))
                deltaPosition += Vector3d::up;
            if (yunutyEngine::Input::isKeyPushed(KeyCode::N))
            {
                ButtonLogicManager::instance->GetLogicMap().at(ButtonLogicManager::eButtonType::NextStage)();
            }
            if (yunutyEngine::Input::isKeyPushed(KeyCode::M))
            {
                ButtonLogicManager::instance->GetLogicMap().at(ButtonLogicManager::eButtonType::Play)();
            }

            if (yunutyEngine::Input::isKeyPushed(KeyCode::NUM_1))
            {
                for (auto each : SaveLoad::GetComponentsFromChildren<StaticMeshWrapper>(Scene::getCurrentScene()))
                    each->GetGI().SetPickingMode(true);
            }
            if (yunutyEngine::Input::isKeyLifted(KeyCode::NUM_1))
            {
                for (auto each : SaveLoad::GetComponentsFromChildren<StaticMeshWrapper>(Scene::getCurrentScene()))
                    each->GetGI().SetPickingMode(false);
            }
            static auto lastMousePos = yunutyEngine::Input::getMouseScreenPosition();

            auto dMousePos = yunutyEngine::Input::getMouseScreenPosition() - lastMousePos;
            lastMousePos = yunutyEngine::Input::getMouseScreenPosition();

            if (yunutyEngine::Input::isKeyDown(KeyCode::MouseRightClick))
            {
                constexpr float rotationSpeedFactor = 0.003f;
                //auto euler = GetTransform()->rotation.Euler();
                //euler.y += dMousePos.x * rotationSpeedFactor;
                //euler.x += dMousePos.y * rotationSpeedFactor;
                Quaternion& rot = GetTransform()->rotation;
                Vector3d newForward =
                    rot.Forward() +
                    dMousePos.x * rotationSpeedFactor * rot.Right() +
                    -dMousePos.y * rotationSpeedFactor * rot.Up();
                Vector3d newRight =
                    //Vector3d::right +
                    rot.Right() +
                    dMousePos.x * rotationSpeedFactor * -rot.Forward();
                Vector3d newUp =
                    rot.Up() +
                    -dMousePos.y * rotationSpeedFactor * -rot.Forward();
                //GetTransform()->rotation = Quaternion::MakeWithAxes(euler);
                GetTransform()->rotation = Quaternion::MakeWithAxes(newRight, newUp, newForward);
            }

            GetTransform()->position += deltaPosition.Normalized() * Time::GetDeltaTime() * cameraSpeed;
#endif
        }
    };
}

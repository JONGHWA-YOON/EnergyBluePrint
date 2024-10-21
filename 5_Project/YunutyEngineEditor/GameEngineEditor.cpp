module;
#include "imgui.h"
#include "YunutyEngine.h"
#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
#include "Walnut/Image.h"
//import walnut;
import ui.EditorLayer;
module editor.GameEngineEditor;

#define MAX_UNDO_SIZE 20
#define MAX_REDO_SIZE 20

using namespace yunutyEngine;

/// 이윤우 : main 함수는 Walnut 헤더에서 정의되며, 에디터의 실질적인 진입 지점은 여기다.
Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
    Walnut::ApplicationSpecification spec;
    spec.Name = "Tree Editor";

    Walnut::Application* app = new GameEngineEditor(spec);
    app->PushLayer<EditorLayer>();
    app->SetMenubarCallback([app]()
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New"))
                {

                }
                if (ImGui::MenuItem("Load"))
                {

                }
                if (ImGui::MenuItem("Save", 0, false, static_cast<GameEngineEditor*>(app)->storability))
                {

                }
                if (ImGui::MenuItem("Save As...", 0, false, static_cast<GameEngineEditor*>(app)->storability))
                {

                }
                if (ImGui::MenuItem("Exit"))
                {
                    app->Close();
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("Undo", "Ctrl + Z", false, static_cast<GameEngineEditor*>(app)->undoStack.size() != 0))
                {

                }
                else if (ImGui::MenuItem("Redo", "Ctrl + Shift + Z", false, static_cast<GameEngineEditor*>(app)->redoStack.size() != 0))
                {

                }
                else if (ImGui::MenuItem("Select All", "Ctrl + A"))
                {

                }
                else if (ImGui::MenuItem("Cut", "Ctrl + X", false, static_cast<GameEngineEditor*>(app)->objectSelection))
                {

                }
                else if (ImGui::MenuItem("Copy", "Ctrl + C", false, static_cast<GameEngineEditor*>(app)->objectSelection))
                {

                }
                else if (ImGui::MenuItem("Paste", "Ctrl + V", false, static_cast<GameEngineEditor*>(app)->objectArchived))
                {

                }
                else if (ImGui::MenuItem("Duplicate", "Ctrl + D", false, static_cast<GameEngineEditor*>(app)->objectSelection))
                {

                }
                else if (ImGui::MenuItem("Rename", "F2", false, static_cast<GameEngineEditor*>(app)->objectSelection))
                {

                }
                else if (ImGui::MenuItem("Delete", "Delete", false, static_cast<GameEngineEditor*>(app)->objectSelection))
                {

                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Window"))
            {
                if (ImGui::BeginMenu("Layout"))
                {
                    if (ImGui::MenuItem("Default"))
                    {
                        static_cast<GameEngineEditor*>(app)->ApplyDefaultLayout();
                    }
                    if (ImGui::MenuItem("Save Layout"))
                    {
                        static_cast<GameEngineEditor*>(app)->SaveLayout();
                    }
                    if (ImGui::MenuItem("Load Layout"))
                    {
                        static_cast<GameEngineEditor*>(app)->LoadLayout();
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
        });
    // start game engine
    Scene::LoadScene(new Scene());

    auto camObject1 = Scene::getCurrentScene()->AddGameObject();
    camObject1->setName("gameObject1");
    auto camObject2 = Scene::getCurrentScene()->AddGameObject();
    camObject2->setName("gameObject2");
    auto camObject3 = Scene::getCurrentScene()->AddGameObject(camObject2);
    camObject3->setName("gameObject3");
	auto camObject4 = Scene::getCurrentScene()->AddGameObject(camObject3);
	camObject4->setName("gameObject4");
	auto camObject5 = Scene::getCurrentScene()->AddGameObject(camObject4);
	camObject5->setName("gameObject5");
	auto camObject6 = Scene::getCurrentScene()->AddGameObject(camObject1);
	camObject6->setName("gameObject6");

    //camObject->AddComponent<ShakyCam>()->SetCameraMain();
    //camObject->GetTransform()->position = Vector3d(0, 6, -18);

    //yunutyEngine::YunutyCycle::SingleInstance().Play();
    return app;
}

GameEngineEditor::GameEngineEditor(const Walnut::ApplicationSpecification& applicationSpecification)
    : Walnut::Application(applicationSpecification)
{
    testInstance = this;
    selectedObjectList.reserve(100);
    archivedObjectList.reserve(100);
    undoStack.reserve(MAX_UNDO_SIZE);
    redoStack.reserve(MAX_UNDO_SIZE);
    InitTestDX();
}

GameEngineEditor::~GameEngineEditor()
{
}

void GameEngineEditor::ApplyDefaultLayout()
{
    //applyDefaultLayout = true;
}

void GameEngineEditor::SaveLayout()
{
    //saveLayout = true;
}

void GameEngineEditor::LoadLayout()
{
    //loadLayout = true;
}

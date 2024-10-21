module;
#include "imgui.h"
#include "backends/imgui_impl_dx11.h"
#include "backends/imgui_impl_win32.h"
#include "YunutyEngine.h"
#include <d3d11.h>
#include <tchar.h>
#include <Windows.h>
#include "UIManager.h"
#include "ButtonLogicManager.h"
#include "DebugManager.h"

import utility.DX11Utility;
import test.components.HailKimJungIl;
import htkg.EditorCamera;
import htkg.InGame.Stage;
//import htkg.InGame.Tile;
import ui.DebugPanel;
import htkg.ResourcesID;
module ui.EditorPanel;
import htkg.SaveLoad;

using namespace utility::dx11Utility;

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Main code
int main(int, char**)
{
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
    ::RegisterClassEx(&wc);

    WNDCLASSEX wcRelease = { sizeof(WNDCLASSEX), CS_CLASSDC, yunutyEngine::Input::WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
    ::RegisterClassEx(&wcRelease);

    HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("Dear ImGui DirectX11 Example"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);
    HWND releasehwnd = ::CreateWindow(wcRelease.lpszClassName, _T("Release version window"), WS_OVERLAPPEDWINDOW, 100, 100, 2560, 1440, NULL, NULL, wcRelease.hInstance, NULL);

    // Initialize Direct3D
    if (CreateDeviceD3D(hwnd) == false)
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Show the window
    ::ShowWindow(releasehwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(releasehwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::LoadIniSettingsFromDisk("LayOutSetting/imgui.ini");
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;
    //io.ConfigViewportsNoDefaultParent = true;
    //io.ConfigDockingAlwaysTabBar = true;
    //io.ConfigDockingTransparentPayload = true;
    //io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: Experimental. THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
    //io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI: Experimental.

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    HINSTANCE hDLL{ LoadLibraryA("MZDX11Renderer.dll") };
    if (hDLL == 0)
        throw 0;
    //yunutyEngine::graphics::Renderer::SingleInstance().LoadGraphicsDll(L"YunuDX11Renderer.dll");
    yunutyEngine::graphics::Renderer::SingleInstance().LoadGraphicsDll(L"MZDX11Renderer.dll");
    yunutyEngine::graphics::Renderer::SingleInstance().SetOutputWindow(releasehwnd);
    yunutyEngine::graphics::Renderer::SingleInstance().LoadFiles(resourcesID::ASEpath.c_str());
    yunutyEngine::graphics::Renderer::SingleInstance().LoadFiles(resourcesID::FBXpath.c_str());
    yunutyEngine::graphics::Renderer::SingleInstance().LoadFiles(resourcesID::TexturePath.c_str());

    auto resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
    auto animationKeys = resourceManager->AnimationKeys();
    auto meshKeys = resourceManager->GetMeshKeys();
    auto skelKeys = resourceManager->GetSkeletonKeys();
    auto texKeys = resourceManager->GetTextureKeys();

    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    ID3D11Device* otherDevice{ nullptr };
    ID3D11DeviceContext* otherDC{ nullptr };
    D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &otherDevice, nullptr, &otherDC);

    ID3D11Texture2D* otherTexture{ nullptr };
    ID3D11ShaderResourceView* tempSrv{ nullptr };
    D3D11_TEXTURE2D_DESC desc
    {
        .Width = 100,
        .Height = 100,
        .MipLevels = 1,
        .ArraySize = 1,
        .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .SampleDesc{.Count = 1},
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
        .CPUAccessFlags = 0,
        .MiscFlags = D3D11_RESOURCE_MISC_SHARED,
    };
    otherDevice->CreateTexture2D(&desc, nullptr, &otherTexture);
    ID3D11RenderTargetView* rtv;
    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc
    {
        .Format = desc.Format,
        .ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D,
        .Texture2D{.MipSlice = 0}
    };
    otherDevice->CreateRenderTargetView(otherTexture, &rtvDesc, &rtv);
    float color_red[]{ 0,1,0,1 };
    float color_blue[]{ 0,0,1,1 };
    otherDC->ClearRenderTargetView(rtv, color_red);
    otherDC->Flush();
    HANDLE sharedHandle{ nullptr };
    IDXGIResource* dxgiResource{ nullptr };
    otherTexture->QueryInterface<IDXGIResource>(&dxgiResource);
    dxgiResource->GetSharedHandle(&sharedHandle);
    dxgiResource->Release();

    ID3D11Resource* receivedResource{ nullptr };
    g_pd3dDevice->OpenSharedResource(sharedHandle, IID_PPV_ARGS(&receivedResource));
    ID3D11Texture2D* receivedTexture{ nullptr };
    receivedResource->QueryInterface(&receivedTexture);

    //g_pd3dDevice->CreateShaderResourceView(sharedTexture2D, &srvDesc, &tempSrv);
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc
    {
        .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
        .Texture2D
        {
            .MostDetailedMip = 0,
            .MipLevels = desc.MipLevels,
        },
    };
    g_pd3dDevice->CreateShaderResourceView(receivedTexture, &srvDesc, &tempSrv);
    otherTexture->Release();

    // start game engine
    Scene::LoadScene(new Scene());
    auto editorCamera = Scene::getCurrentScene()->AddGameObject();
    editorCamera->setName("Editor Camera");
    editorCamera->AddComponent<htkg::EditorCamera>()->SetCameraMain();

    // UI 관련 작업 중
    auto canvas = Scene::getCurrentScene()->AddGameObject();
    canvas->setName("Canvas");

    auto titleCanvas = Scene::getCurrentScene()->AddGameObject(canvas);
    titleCanvas->setName("Title Canvas");
    UIManager::GetInstance()->CreateImageButton(0.0f, 0.0f, 1.01f, 1.0f, true, "Title BackGroud", titleCanvas, "Textures\\Title.dds", ButtonLogicManager::eButtonType::Back);
    titleCanvas->SetSelfActive(true);


    auto mainMenuCanvas = Scene::getCurrentScene()->AddGameObject(canvas);
    mainMenuCanvas->setName("Main Menu Canvas");
    UIManager::GetInstance()->CreateImage(0.0f, 0.0f, 1.01f, 1.0f, true, "MainMenu BackGroud", mainMenuCanvas, "Textures\\MainMenu.dds");
    UIManager::GetInstance()->CreateImageButton(0.68f, 0.3f, 0.2f, 0.13f, true, "GameStart Button", mainMenuCanvas, "Textures\\GameStartButton.dds", ButtonLogicManager::eButtonType::GameStart);
    UIManager::GetInstance()->CreateImageButton(0.68f, 0.01f, 0.2f, 0.13f, true, "Credits Button", mainMenuCanvas, "Textures\\CreditsButton.dds", ButtonLogicManager::eButtonType::Credit);
    UIManager::GetInstance()->CreateImageButton(0.68f, -0.35f, 0.2f, 0.13f, true, "GameQuit Button", mainMenuCanvas, "Textures\\QuitButton.dds", ButtonLogicManager::eButtonType::GameQuit);
    mainMenuCanvas->SetSelfActive(false);

    auto creditCanvas = Scene::getCurrentScene()->AddGameObject(canvas);
    creditCanvas->setName("Credit Canvas");
    UIManager::GetInstance()->CreateImage(0.0f, 0.0f, 1.01f, 1.0f, true, "Credits BackGroud", creditCanvas, "Textures\\Credits.dds");
    UIManager::GetInstance()->CreateImageButton(-0.89f, 0.87f, 0.1f, 0.13f, true, "Back Button", creditCanvas, "Textures\\BackButton.dds", ButtonLogicManager::eButtonType::Back);
    creditCanvas->SetSelfActive(false);

    auto stageSelectCanvas = Scene::getCurrentScene()->AddGameObject(canvas);
    stageSelectCanvas->setName("Stage Select Canvas");
    UIManager::GetInstance()->CreateImage(0.0f, 0.0f, 1.01f, 1.0f, true, "StageSelect BackGroud", stageSelectCanvas, "Textures\\StageSelect.dds");
    UIManager::GetInstance()->CreateImageButton(0.51f, 0.05f, 0.2f, 0.270f, true, "UnLock Stage", stageSelectCanvas, "Textures\\EmptyStageBoard.dds");
    UIManager::GetInstance()->CreateImageButton(0.0f, 0.06f, 0.3f, 0.41f, true, "Stage Button", stageSelectCanvas, "Textures\\StageBoard.dds", ButtonLogicManager::eButtonType::Stage);
    UIManager::GetInstance()->CreateImageButton(-0.89f, 0.87f, 0.1f, 0.13f, true, "Back Button", stageSelectCanvas, "Textures\\BackButton.dds", ButtonLogicManager::eButtonType::Back);
    UIManager::GetInstance()->CreateImageButton(0.0f, -0.65f, 0.15f, 0.15f, true, "Play Button", stageSelectCanvas, "Textures\\PlayButton.dds", ButtonLogicManager::eButtonType::Play);
    stageSelectCanvas->SetSelfActive(false);

    auto ingameMenuCanvas = Scene::getCurrentScene()->AddGameObject(canvas);
    ingameMenuCanvas->setName("Ingame Menu Canvas");
    UIManager::GetInstance()->CreateImage(0.0f, 0.03f, 0.4f, 1.03f, true, "Ingame Menu Panel", ingameMenuCanvas, "Textures\\IngameMenuPanel.dds");
    UIManager::GetInstance()->CreateImageButton(0.0f, 0.3f, 0.21f, 0.132f, true, "ContinueButton", ingameMenuCanvas, "Textures\\ContinueButton.dds", ButtonLogicManager::eButtonType::Continue);
    UIManager::GetInstance()->CreateImageButton(0.0f, -0.15f, 0.17f, 0.1f, true, "ReStartButton", ingameMenuCanvas, "Textures\\ReStartButton.dds", ButtonLogicManager::eButtonType::Restart);
    UIManager::GetInstance()->CreateImageButton(0.0f, -0.625f, 0.1f, 0.1f, true, "MenuQuitButton", ingameMenuCanvas, "Textures\\MenuQuitButton.dds", ButtonLogicManager::eButtonType::MenuQuit);
    ingameMenuCanvas->SetSelfActive(false);

    auto ingameUICanvas = Scene::getCurrentScene()->AddGameObject(canvas);
    UIManager::GetInstance()->CreateImageButton(0.92f, 0.9f, 0.055f, 0.076f, true, "Ingame Menu", ingameUICanvas, "Textures\\IngameMenu.dds", ButtonLogicManager::eButtonType::Menu);
    UIManager::GetInstance()->CreateImageButton(0.0f, -0.8f, 0.1f, 0.08f, true, "Run Button", ingameUICanvas, "Textures\\RunButton.dds", ButtonLogicManager::eButtonType::Run);
    ingameUICanvas->setName("Ingame UI Canvas");
    ingameUICanvas->SetSelfActive(false);

    auto stageClearCanvas = Scene::getCurrentScene()->AddGameObject(canvas);
    UIManager::GetInstance()->CreateImage(0.0f, 0.0f, 1.01f, 1.0f, true, "StageBoard BackGroud", stageClearCanvas, "Textures\\GameClearBoard.dds");
    UIManager::GetInstance()->CreateImageButton(0.0f, -0.325f, 0.23f, 0.1f, true, "NextStage Button", stageClearCanvas, "Textures\\NextStageButton.dds", ButtonLogicManager::eButtonType::NextStage);
    UIManager::GetInstance()->CreateImageButton(0.0f, -0.7f, 0.13f, 0.085f, true, "ReStart Button", stageClearCanvas, "Textures\\ReStartButton2.dds", ButtonLogicManager::eButtonType::Restart);
    stageClearCanvas->setName("Stage Clear Canvas");
    stageClearCanvas->SetSelfActive(false);

    auto stageFailCanvas = Scene::getCurrentScene()->AddGameObject(canvas);
    UIManager::GetInstance()->CreateImage(0.0f, 0.0f, 1.01f, 1.0f, true, "StageBoard BackGroud", stageFailCanvas, "Textures\\GameClearBoard.dds");
    UIManager::GetInstance()->CreateImageButton(0.0f, -0.475f, 0.355f, 0.180f, true, "ReStart Button", stageFailCanvas, "Textures\\ReStartButton2.dds", ButtonLogicManager::eButtonType::Restart);
    stageFailCanvas->setName("Stage Fail Canvas");
    stageFailCanvas->SetSelfActive(false);

    auto inGameObjectList = Scene::getCurrentScene()->AddGameObject();
    inGameObjectList->setName("InGame");
    inGameObjectList->SetSelfActive(false);

    // 텍스트 버튼
	// 현재 부모에 집어넣으면 이상하게 동작해서 부모 이름은 넣지만 부모로 들어가진 않음
	UIManager::GetInstance()->CreateTextButton(0.0f, 0.0f, 0.1f, 0.1f, false, "Text Button", mainMenuCanvas, (TCHAR*)L"Please", ButtonLogicManager::eButtonType::None);

    // 테스트용 초기화 코드
    GameObject* stage = Scene::getCurrentScene()->AddGameObject(inGameObjectList);
    Stage* stageComp = stage->AddComponent<Stage>();
    stageComp->SetHeight(5);
    stageComp->SetWidth(5);

    //testObjectComp->GetGI().LoadMesh("ASEFile/genji_max2.ASE");
    //testObjectComp->GetGI().LoadMesh("ASEFile/Rob02Trang.ASE");
    //testObjectComp->GetGI().LoadMesh("FBXFile/Rob02Trang.fbx");
    //testObjectComp->GetGI().LoadNormalMap("Textures/000000002406_b_reverse.dds");
    //testObjectComp->GetGI().LoadDiffuseMap("Textures/000000002405_reverse.dds");
    //testObjectComp->GetTransform()->SetWorldPosition(Vector3d(0.0f, 0.0f, 0.0f));
    //testObjectComp->GetTransform()->scale = Vector3d(0.3f, 0.3f, 0.0f);

    auto testObject = Scene::getCurrentScene()->AddGameObject();
    auto testObjectComp = testObject->AddComponent<yunutyEngine::graphics::StaticMesh>();
    //testObjectComp->GetGI().LoadMesh("ASEFile/Rob02Trang.ASE");
    //testObjectComp->GetGI().LoadNormalMap("Textures/Rob02_Normal_reverse.dds");
    //testObjectComp->GetGI().LoadDiffuseMap("Textures/Rob02Blue_AlbedoTransparency_reverse.dds");
    //testObjectComp->GetGI().LoadMesh("ASEFile/Ground_Small_1.ASE");
    //testObjectComp->GetGI().LoadNormalMap("Textures/Ground_NM_reverse.dds");
    //testObjectComp->GetGI().LoadDiffuseMap("Textures/Ground_ALB_reverse.dds");
    //testObjectComp->GetTransform()->SetWorldPosition(Vector3d(0.0f, 0.0f, 0.0f));
    //testObjectComp->GetTransform()->scale = Vector3d(0.3f, 0.3f, 0.0f);

    SaveLoad::stageParent = inGameObjectList;
    SaveLoad::LoadScene();
    yunutyEngine::YunutyCycle::SingleInstance().Play();
    //camObject->AddComponent<ShakyCam>()->SetCameraMain();
    //camObject->GetTransform()->position = Vector3d(0, 6, -18);

    //yunutyEngine::YunutyCycle::SingleInstance().Play();
    // Main loop
    bool done = false;
    bool editorBool = true;
    std::vector<EditorPanel*> panelList;
    for (size_t i = 0; i < (size_t)Panel_List::COUNT * PANEL_LIST_INTERVAL; i += PANEL_LIST_INTERVAL)
    {
        panelList.push_back(EditorPanel::CreatePanel((Panel_List)i));
        panelList[i / PANEL_LIST_INTERVAL]->Init();
    }
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        //if (show_demo_window)
        //    ImGui::ShowDemoWindow(&show_demo_window);

        //// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        //{
        //    static float f = 0.0f;
        //    static int counter = 0;

        //    ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        //    ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        //    ImGui::Checkbox("Demo Window", &show_demo_window);      // E`dit bools storing our window open/close state
        //    ImGui::Checkbox("Another Window", &show_another_window);

        //    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        //    ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        //    if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        //    {
        //        counter++;
        //        otherDC->ClearRenderTargetView(rtv, (counter % 2 == 0) ? color_red : color_blue);
        //        otherDC->Flush();
        //    }
        //    ImGui::SameLine();
        //    ImGui::Text("counter = %d", counter);

        //    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        //    ImGui::Text("pointer = %p", (yunutyEngine::graphics::Renderer::SingleInstance().QueryD3D11RenderOutputSRV()));
        //    ImGui::Image(static_cast<void*>(tempSrv), { 1920,1080 });
            ////ImGui::Image(static_cast<void*>(yunutyEngine::graphics::Renderer::SingleInstance().QueryD3D11RenderOutputSRV()), { 1920,1080 });
        //    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        //    ImGui::End();
        //}

        //// 3. Show another simple window.
        //if (show_another_window)
        //{
        //    ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        //    ImGui::Text("Hello from another window!");
        //    if (ImGui::Button("Close Me"))
        //        show_another_window = false;
        //    ImGui::Image(yunutyEngine::graphics::Renderer::SingleInstance().QueryD3D11RenderOutputSRV(), { 100,100 });
        //    ImGui::End();
        //}

        //ImGui::Begin("YunutyEditor", &editorBool);
        //if (ImGui::BeginMenu("File"))
        //{
        //    if (ImGui::MenuItem("New"))
        //    {
        //
        //    }
        //    if (ImGui::MenuItem("Load"))
        //    {
        //
        //    }
        //    /*if (ImGui::MenuItem("Save", 0, false, static_cast<GameEngineEditor*>(app)->storability))
        //    {
        //
        //    }
        //    if (ImGui::MenuItem("Save As...", 0, false, static_cast<GameEngineEditor*>(app)->storability))
        //    {
        //
        //    }*/
        //    /*if (ImGui::MenuItem("Exit"))
        //    {
        //        app->Close();
        //    }*/
        //    ImGui::EndMenu();
        //}

        //if (ImGui::BeginMenu("Edit"))
        //{
        //    /*if (ImGui::MenuItem("Undo", "Ctrl + Z", false, static_cast<GameEngineEditor*>(app)->undoStack.size() != 0))
        //    {
        //
        //    }
        //    else if (ImGui::MenuItem("Redo", "Ctrl + Shift + Z", false, static_cast<GameEngineEditor*>(app)->redoStack.size() != 0))
        //    {
        //
        //    }
        //    else if (ImGui::MenuItem("Select All", "Ctrl + A"))
        //    {
        //
        //    }
        //    else if (ImGui::MenuItem("Cut", "Ctrl + X", false, static_cast<GameEngineEditor*>(app)->objectSelection))
        //    {
        //
        //    }
        //    else if (ImGui::MenuItem("Copy", "Ctrl + C", false, static_cast<GameEngineEditor*>(app)->objectSelection))
        //    {
        //
        //    }
        //    else if (ImGui::MenuItem("Paste", "Ctrl + V", false, static_cast<GameEngineEditor*>(app)->objectArchived))
        //    {
        //
        //    }
        //    else if (ImGui::MenuItem("Duplicate", "Ctrl + D", false, static_cast<GameEngineEditor*>(app)->objectSelection))
        //    {
        //
        //    }
        //    else if (ImGui::MenuItem("Rename", "F2", false, static_cast<GameEngineEditor*>(app)->objectSelection))
        //    {
        //
        //    }
        //    else if (ImGui::MenuItem("Delete", "Delete", false, static_cast<GameEngineEditor*>(app)->objectSelection))
        //    {
        //
        //    }*/
        //    ImGui::EndMenu();
        //}

        //if (ImGui::BeginMenu("Window"))
        //{
        //    if (ImGui::BeginMenu("Layout"))
        //    {
        //        /*if (ImGui::MenuItem("Default"))
        //        {
        //            static_cast<GameEngineEditor*>(app)->ApplyDefaultLayout();
        //        }
        //        if (ImGui::MenuItem("Save Layout"))
        //        {
        //            static_cast<GameEngineEditor*>(app)->SaveLayout();
        //        }
        //        if (ImGui::MenuItem("Load Layout"))
        //        {
        //            static_cast<GameEngineEditor*>(app)->LoadLayout();
        //        }*/
        //        ImGui::EndMenu();
        //    }
        //    ImGui::EndMenu();
        //}
        //ImGui::End();

 		if (ImGui::IsKeyDown(ImGuiKey_Home) && DebugManager::GetInstance()->m_isPlayMode == false)
 		{
             DebugManager::GetInstance()->m_isPlayMode = true;
 		}
 		else if (ImGui::IsKeyDown(ImGuiKey_End) && DebugManager::GetInstance()->m_isPlayMode == true)
 		{
             DebugManager::GetInstance()->m_isPlayMode = false;
 		}

        if (DebugManager::GetInstance()->m_isPlayMode == false)
        {
            for (int i = 0; i < panelList.size(); i++)
            {
                if (i != 4)
                {
                    panelList[i]->DrawPanel();
                }
            }
        }
        else
        {
            panelList[4]->DrawPanel();
        }


        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        //g_pSwapChain->Present(1, 0); // Present with vsync
        g_pSwapChain->Present(0, 0); // Present without vsync
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}


// Helper functions



#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0 // From Windows SDK 8.1+ headers
#endif

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    case WM_DPICHANGED:
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
        {
            //const int dpi = HIWORD(wParam);
            //printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
            const RECT* suggested_rect = (RECT*)lParam;
            ::SetWindowPos(hWnd, NULL, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
        }
        break;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

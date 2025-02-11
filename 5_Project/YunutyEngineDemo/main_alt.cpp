// Dear ImGui: standalone example application for DirectX 11
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "YunutyEngine.h"
#include "ShakyCam.h"   
#include <d3d11.h>
#include <tchar.h>
#include <map>

// Data
static ID3D11Device* g_pd3dDevice = NULL;
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

ID3D11ShaderResourceView* GetSRV(void* handle)
{
	static std::map<void*, ID3D11ShaderResourceView*> srvs;
	if (srvs.find(handle) == srvs.end())
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc
		{
			.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
			.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
			.Texture2D
			{
				.MostDetailedMip = 0,
				.MipLevels = 1,
			},
		};
		ID3D11Resource* receivedResource{ nullptr };
		ID3D11ShaderResourceView* srv{ nullptr };
		g_pd3dDevice->OpenSharedResource(handle, IID_PPV_ARGS(&receivedResource));
		g_pd3dDevice->CreateShaderResourceView(receivedResource, &srvDesc, &srv);
		srvs[handle] = srv;
	}
	return srvs[handle];
}
// Main code
int main(int, char**)
{
	// Create application window
	//ImGui_ImplWin32_EnableDpiAwareness();
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
	::RegisterClassEx(&wc);
	HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("Dear ImGui DirectX11 Example"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);
	HWND releaseHwnd = ::CreateWindow(wc.lpszClassName, _T("Release version game window"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	::ShowWindow(releaseHwnd, SW_SHOWDEFAULT);
	::UpdateWindow(releaseHwnd);

	// Initialize Direct3D
	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClass(wc.lpszClassName, wc.hInstance);
		throw std::runtime_error(std::string("failed to create d3d device!"));
	}

	// Show the window
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
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
	bool show_another_window = true;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	//yunutyEngine::graphics::Renderer::SingleInstance().LoadGraphicsDll(L"YunuDX11Renderer.dll");
	yunutyEngine::graphics::Renderer::SingleInstance().LoadGraphicsDll(L"MZDX11Renderer.dll");
	yunutyEngine::graphics::Renderer::SingleInstance().SetOutputWindow(releaseHwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());
	auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	camObj->AddComponent<ShakyCam>()->SetCameraMain();

	/*class FlappyBird : public Component
	{
	protected:
		float gravity = -9.81;
		float currentSpeed{ 0 };
		float flapSpeed = 25;
		virtual void Update()override
		{
			GetTransform()->position.y += currentSpeed * Time::GetDeltaTime();
			currentSpeed += gravity * Time::GetDeltaTime();
			if (Input::isKeyPushed(KeyCode::Space))
				currentSpeed = flapSpeed;
		}
	};*/
	auto staticMeshObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	auto staticMesh = staticMeshObj->AddComponent<yunutyEngine::graphics::StaticMesh>();
	//staticMeshObj->AddComponent<FlappyBird>();
	staticMesh->GetGI().LoadMesh("ASEFile/genji_blender.ASE");
	staticMesh->GetGI().LoadDiffuseMap("Textures/000000002405_reverse.dds");
	staticMesh->GetGI().LoadNormalMap("Textures/000000002406_b_reverse.dds");

	yunutyEngine::YunutyCycle::SingleInstance().autoRendering = false;
	yunutyEngine::YunutyCycle::SingleInstance().Play();

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
		.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
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
		.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
		.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
		.Texture2D
		{
			.MostDetailedMip = 0,
			.MipLevels = desc.MipLevels,
		},
	};
	g_pd3dDevice->CreateShaderResourceView(receivedTexture, &srvDesc, &tempSrv);
	otherTexture->Release();
	map<void*, ID3D11ShaderResourceView*> srvs;


	// Main loop
	bool done = false;
	while (!done)
	{
		yunutyEngine::graphics::Renderer::SingleInstance().ManualRender();
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

		//Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			{
				counter++;
				otherDC->ClearRenderTargetView(rtv, (counter % 2 == 0) ? color_red : color_blue);
				otherDC->Flush();
			}
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Text("pointer = %p", (yunutyEngine::graphics::Renderer::SingleInstance().QuerySharedOutputHandle()));
			ImGui::Image(GetSRV(yunutyEngine::graphics::Renderer::SingleInstance().QuerySharedOutputHandle()), { 1280,800 });
			//ImGui::Image(static_cast<void*>(tempSrv), { 1920,1080 });
			//ImGui::Image(static_cast<void*>(yunutyEngine::graphics::Renderer::SingleInstance().QueryD3D11RenderOutputSRV()), { 1920,1080 });
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			ImGui::Image(GetSRV(yunutyEngine::graphics::Renderer::SingleInstance().QuerySharedOutputHandle()), { 1280,800 });
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			//ImGui::Image(GetSRV(yunutyEngine::graphics::Renderer::SingleInstance().QuerySharedOutputHandle()), { 100,100 });
			ImGui::End();
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

		if (yunutyEngine::Input::isKeyDown(KeyCode::MouseLeftClick))
		{
			//yunutyEngine::graphics::Renderer::SingleInstance().InvokeClickEvent(x, y);
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

bool CreateDeviceD3D(HWND hWnd)
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
	pBackBuffer->Release();
}

void CleanupRenderTarget()
{
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

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


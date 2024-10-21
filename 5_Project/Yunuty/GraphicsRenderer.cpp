#include <d3d11.h>
#include <unordered_map>
#include "GraphicsRenderer.h"
#include "YunutyEngine.h"
#include "_YunuGIObjects.h"
//#include "IYunuGI3DRenderer.h"
#include "YunuGraphicsInterface.h"
#include "YunutyGraphicsEventHandler.h"
#include "YunutyRenderable.h"
#include <filesystem>
#include <assert.h>

using namespace yunutyEngine::graphics;
unique_ptr<Renderer> yunutyEngine::graphics::Renderer::instance = nullptr;

Renderer& yunutyEngine::graphics::Renderer::SingleInstance()
{
    if (!instance.get())
        instance = make_unique<Renderer>();
    return *instance;
}
void yunutyEngine::graphics::Renderer::Render()
{
    auto renderer = _YunuGIObjects::SingleInstance().renderer.Get();
    assert(renderer != nullptr, L"그래픽스 렌더러가 없는 상태입니다!");
    //renderer->BeginRender();
    renderer->Render();
    afterRenderAction();
    //renderer->EndRender();
}
void yunutyEngine::graphics::Renderer::Update(float delta)
{
    auto renderer = _YunuGIObjects::SingleInstance().renderer.Get();
    renderer->Update(delta);
}
void yunutyEngine::graphics::Renderer::LoadGraphicsDll(const wstring& dllFileName)
{
    _YunuGIObjects::SingleInstance().LoadDll(dllFileName);
}
void yunutyEngine::graphics::Renderer::SetOutputWindow(const HWND& hwnd)
{
    auto renderer = _YunuGIObjects::SingleInstance().renderer.Get();
    assert(renderer != nullptr, L"그래픽스 렌더러가 없는 상태입니다!");
    renderer->SetOutputWindow(reinterpret_cast<unsigned long long>(hwnd));
}
void yunutyEngine::graphics::Renderer::LoadFiles(const char* rootPath)
{
    std::stack<std::filesystem::path> folders_stack;
    folders_stack.push(rootPath);

    while (!folders_stack.empty()) {
        std::filesystem::path current_folder = folders_stack.top();
        folders_stack.pop();

        for (const auto& entry : std::filesystem::directory_iterator(current_folder)) {
            if (std::filesystem::is_directory(entry)) {
                folders_stack.push(entry.path());
            }
            else if (std::filesystem::is_regular_file(entry)) {
                LoadFile(entry.path().string().c_str());
            }
        }
    }
}
void yunutyEngine::graphics::Renderer::LoadFile(const char* fileName)
{
    _YunuGIObjects::SingleInstance().resourceManager->LoadFile(fileName);
}
void yunutyEngine::graphics::Renderer::SetAfterRenderAction(const function<void()>& action)
{
    afterRenderAction = action;
}
void* yunutyEngine::graphics::Renderer::QuerySharedOutputHandle()
{
    auto renderer = _YunuGIObjects::SingleInstance().renderer.Get();
    return renderer->QuerySharedOutputHandle();
}
void yunutyEngine::graphics::Renderer::InvokeClickEvent(float x, float y)
{
    auto selected = _YunuGIObjects::SingleInstance().eventHandler.Get()->Click(x, y);
    yunutyEngine::graphics::EventHandler::CallEvents(selected, [](yunutyEngine::graphics::EventHandler* each) {each->onClick(); });
}
void yunutyEngine::graphics::Renderer::InvokeHoverEvent(float x, float y)
{
    auto selected = _YunuGIObjects::SingleInstance().eventHandler.Get()->Click(x, y);
    yunutyEngine::graphics::EventHandler::CallEvents(selected, [](yunutyEngine::graphics::EventHandler* each) {each->onHover(); });
}
Renderable<yunuGI::IRenderable>* yunutyEngine::graphics::Renderer::ClickRenderable(float x, float y)
{
    auto selected = _YunuGIObjects::SingleInstance().eventHandler.Get()->Click(x, y);
    auto itr = yunutyEngine::graphics::Renderable<yunuGI::IRenderable>::yunuGIWrapperMap.find(selected);
    if (itr == yunutyEngine::graphics::Renderable<yunuGI::IRenderable>::yunuGIWrapperMap.end())
        return nullptr;

    return (*itr).second;
}
const yunuGI::IResourceManager* yunutyEngine::graphics::Renderer::GetResourceManager()
{
    return _YunuGIObjects::SingleInstance().resourceManager.Get();
}
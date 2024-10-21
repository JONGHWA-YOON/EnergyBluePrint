module;
#include <Windows.h>
#include "YunutyEngine.h"
export module graphics.CallbackHandler;

export namespace graphics {
    class CallbackHandler
    {
    public:
        static void RegisterIngameHwnd(HWND hwnd)
        {
            yunutyEngine::graphics::Renderer::SingleInstance().SetOutputWindow(hwnd);
        }
    private:
        CallbackHandler() {}
    };
}
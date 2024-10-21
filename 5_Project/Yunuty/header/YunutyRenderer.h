#pragma once
#include <Windows.h>
#include <vector>
#include <unordered_set>
#include <functional>
#include <memory>

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif
struct ID3D11Device;
struct ID3D11DeviceContext;

namespace Yunu3DAPI
{
    class IYunuGI3DRenderer;
}
using namespace std;
namespace YunutyEngine
{
    namespace Graphics
    {
        class YUNUTY_API YunutyRenderer
        {
        public:
            static YunutyRenderer& GetInstance();
            void Render();
            void Update(float delta);
            void LoadGraphicsDll(const wstring& dllFileName);
            void SetOutputWindow(const HWND& hwnd);
            void SetAfterRenderAction(const function<void()>& action);
            ID3D11Device* QueryD3d11Device();
            ID3D11DeviceContext* QueryD3d11DeviceContext();
        private:
            static unique_ptr<YunutyRenderer> instance;
            Yunu3DAPI::IYunuGI3DRenderer* graphicsRenderer{nullptr};
            function<void()> afterRenderAction{ []() {} };
        };
    }
}

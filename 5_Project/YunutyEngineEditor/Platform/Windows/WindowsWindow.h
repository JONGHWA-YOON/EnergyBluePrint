/// 2023.06.29	김상준

/// EditorWindow 의 Windows 운영체제용 구체화 클래스
/// Windows 관련된 내용으로 창을 생성

#pragma once

#include "EditorWindow.h"
#include "Renderer/GraphicsContext.h"

#include <GLFW/glfw3.h>

class WindowsWindow
	: public EditorWindow
{
public:
	WindowsWindow(const WindowSpecification& specification);
	virtual ~WindowsWindow();

	virtual void Init() override;
	virtual void ProcessEvents() override;
	virtual void OnUpdate() override;

	virtual unsigned int GetWidth() const override { return winData.width; }
	virtual unsigned int GetHeight() const override { return winData.height; }

	virtual std::pair<uint32_t, uint32_t> GetSize() const override { return { winData.width, winData.height }; }
	virtual std::pair<float, float> GetWindowPos() const override;

	// Window attributes
	virtual void SetEventCallback(const EventCallbackFn& callback) override { winData.eventCallback = callback; }
	virtual void SetVSync(bool enabled) override;
	virtual bool IsVSync() const override;
	virtual void SetResizable(bool resizable) const override;

	virtual void Maximize() override;
	virtual void CenterWindow() override;

	virtual const std::string& GetTitle() const override { return winData.title; }
	virtual void SetTitle(const std::string& title) override;

	virtual void* GetNativeWindow() const { return window; }

private:
	void Shutdown();

private:
	GLFWwindow* window;	// 이 부분이 추후 Win32 용으로 제작한 윈도우로 변경해야 할 것으로 보임
	GLFWcursor* ImGuiMouseCursors[9] = {};
	WindowSpecification specification;
	
	struct WindowData
	{
		std::string title;
		uint32_t width;
		uint32_t height;

		EventCallbackFn eventCallback;
	};

	WindowData winData;

	Scope<GraphicsContext> graphicsContext;
};


#include "Base.h"
#include "Input.h"
#include "ApplicationEvents.h"
#include "MouseEvents.h"
#include "KeyEvents.h"
#include "Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLContext.h"

#include "WindowsWindow.h"

static bool GLFWInitialized = false;

static void GLFWErrorCallback(int error, const char* description)
{
}

EditorWindow* EditorWindow::Create(const WindowSpecification& specification)
{
	return new WindowsWindow(specification);
}

WindowsWindow::WindowsWindow(const WindowSpecification& specification)
{

}

WindowsWindow::~WindowsWindow()
{
	Shutdown();
}

void WindowsWindow::Init()
{
	winData.title = props.title;
	winData.width = props.width;
	winData.height = props.height;

	if (s_GLFWWindowCount == 0)
	{
		int success = glfwInit();
		glfwSetErrorCallback(GLFWErrorCallback);
	}

	{
#if defined(_DEBUG)
		if (Renderer::GetAPI() == RendererAPI::API::OpenGL)
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
		window = glfwCreateWindow((int)props.width, (int)props.height, winData.title.c_str(), nullptr, nullptr);
		++s_GLFWWindowCount;
	}

	graphicsContext = GraphicsContext::Create(window);
	graphicsContext->Init();

	glfwSetWindowUserPointer(window, &winData);
	SetVSync(true);

	// Set GLFW callbacks
	glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
	data.width = width;
	data.height = height;

	WindowResizeEvent event(width, height);
	data.eventCallback(event);
		});

	glfwSetWindowCloseCallback(window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
	WindowCloseEvent event;
	data.eventCallback(event);
		});

	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

	switch (action)
	{
		case GLFW_PRESS:
		{
			KeyPressedEvent event(key, 0);
			data.eventCallback(event);
			break;
		}
		case GLFW_RELEASE:
		{
			KeyReleasedEvent event(key);
			data.eventCallback(event);
			break;
		}
		case GLFW_REPEAT:
		{
			KeyPressedEvent event(key, true);
			data.eventCallback(event);
			break;
		}
	}
		});

	glfwSetCharCallback(window, [](GLFWwindow* window, unsigned int keycode)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

	KeyTypedEvent event(keycode);
	data.eventCallback(event);
		});

	glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

	switch (action)
	{
		case GLFW_PRESS:
		{
			MouseButtonPressedEvent event(button);
			data.eventCallback(event);
			break;
		}
		case GLFW_RELEASE:
		{
			MouseButtonReleasedEvent event(button);
			data.eventCallback(event);
			break;
		}
	}
		});

	glfwSetScrollCallback(window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

	MouseScrolledEvent event((float)xOffset, (float)yOffset);
	data.eventCallback(event);
		});

	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

	MouseMovedEvent event((float)xPos, (float)yPos);
	data.eventCallback(event);
		});
}

void WindowsWindow::Shutdown()
{

	glfwDestroyWindow(window);
	--s_GLFWWindowCount;

	if (s_GLFWWindowCount == 0)
	{
		glfwTerminate();
	}
}

void WindowsWindow::OnUpdate()
{
	glfwPollEvents();
	graphicsContext->SwapBuffers();
}

void WindowsWindow::SetVSync(bool enabled)
{
	if (enabled)
		glfwSwapInterval(1);
	else
		glfwSwapInterval(0);

	winData.VSync = enabled;
}

bool WindowsWindow::IsVSync() const
{
	return winData.VSync;
}


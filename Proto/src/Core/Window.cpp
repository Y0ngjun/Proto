#include <stdexcept>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Window.h"

namespace Proto
{
	Window::Window(int width, int height, const char* title)
		: m_Window(nullptr),
		m_Width(width),
		m_Height(height)
	{
		if (!glfwInit())
		{
			throw std::runtime_error("GLFW initialization failed.");
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// 주 모니터 해상도 가져오기
		GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);
		
		m_Width = videoMode->width;
		m_Height = videoMode->height;

		// 윈도우를 최대화(전체 화면 꽉 차게) 상태로 시작하도록 설정
		glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

		m_Window = glfwCreateWindow(m_Width, m_Height, title, nullptr, nullptr);
		if (!m_Window)
		{
			glfwTerminate();
			throw std::runtime_error("Failed to create GLFW window.");
		}

		glfwMakeContextCurrent(m_Window);
		glfwSwapInterval(1);

		if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
		{
			glfwDestroyWindow(m_Window);
			glfwTerminate();
			throw std::runtime_error("GLAD initialization failed.");
		}

		glfwSetWindowUserPointer(m_Window, this);
		glfwSetFramebufferSizeCallback(m_Window, FramebufferSizeCallback);
	}

	Window::~Window()
	{
		if (m_Window)
		{
			glfwDestroyWindow(m_Window);
			m_Window = nullptr;
		}

		glfwTerminate();
	}

	void Window::PollEvents() const
	{
		glfwPollEvents();
	}

	void Window::SwapBuffers() const
	{
		glfwSwapBuffers(m_Window);
	}

	bool Window::ShouldClose() const
	{
		return glfwWindowShouldClose(m_Window);
	}

	void Window::SetShouldClose(bool shouldClose) const
	{
		glfwSetWindowShouldClose(m_Window, shouldClose ? GLFW_TRUE : GLFW_FALSE);
	}

	void Window::SetVSync(bool enabled) const
	{
		glfwSwapInterval(enabled ? 1 : 0);
	}

	int Window::GetWidth() const
	{
		return m_Width;
	}

	int Window::GetHeight() const
	{
		return m_Height;
	}

	void Window::SetTitle(const char* title) const
	{
		glfwSetWindowTitle(m_Window, title);
	}

	GLFWwindow* Window::GetNativeWindow() const
	{
		return m_Window;
	}

	void Window::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
	{
		auto* owner = static_cast<Window*>(glfwGetWindowUserPointer(window));
		if (owner)
		{
			owner->m_Width = width;
			owner->m_Height = height;
		}

		glViewport(0, 0, width, height);
	}
}

/*
 * GLFW 기반의 윈도우 생성 및 컨텍스트 관리를 담당하는 클래스입니다.
 * 화면 크기 조절, VSync 설정, 이벤트 폴링 등의 기능을 제공합니다.
 */

#pragma once

struct GLFWwindow;

namespace Proto
{
	class Window
	{
	public:
		Window(int width = 1280, int height = 720, const char* title = "Proto Engine");
		~Window();

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		void PollEvents() const;
		void SwapBuffers() const;
		bool ShouldClose() const;
		void SetShouldClose(bool shouldClose) const;

		void SetVSync(bool enabled) const;
		int GetWidth() const;
		int GetHeight() const;
		void SetTitle(const char* title) const;

		GLFWwindow* GetNativeWindow() const;

	private:
		static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

		GLFWwindow* m_Window;
		int m_Width;
		int m_Height;

		static int m_WindowCount;
	};
}

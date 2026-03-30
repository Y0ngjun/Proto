#pragma once

struct GLFWwindow;

namespace Proto
{
	class Window
	{
	public:
		Window(int width = 1280, int height = 720, const char* title = "Proto Engine Test");
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
	};
}

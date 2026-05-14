#include <stdexcept>
#include <GLFW/glfw3.h>
#include "RawInput.h"
#include <cstring>

namespace Proto
{
	GLFWwindow* RawInput::s_Window = nullptr;
	static bool s_KeyStateBuffer[512];
	static bool s_PreviousKeyStateBuffer[512];

	void RawInput::Initialize(GLFWwindow* window)
	{
		s_Window = window;
		std::memset(s_KeyStateBuffer, 0, sizeof(s_KeyStateBuffer));
		std::memset(s_PreviousKeyStateBuffer, 0, sizeof(s_PreviousKeyStateBuffer));
	}

	void RawInput::Update()
	{
		if (!s_Window) return;

		// 이전 상태 보관
		std::memcpy(s_PreviousKeyStateBuffer, s_KeyStateBuffer, sizeof(s_KeyStateBuffer));

		// 모든 키 상태 한 번에 폴링 (0 ~ 511 범위)
		for (int i = 0; i < 512; i++)
		{
			s_KeyStateBuffer[i] = glfwGetKey(s_Window, i) == GLFW_PRESS;
		}
	}

	bool RawInput::GetKey(KeyCode key)
	{
		if (key < 0 || key >= 512) return false;
		return s_KeyStateBuffer[key];
	}

	bool RawInput::GetKeyDown(KeyCode key)
	{
		if (key < 0 || key >= 512) return false;
		return s_KeyStateBuffer[key] && !s_PreviousKeyStateBuffer[key];
	}

	bool RawInput::GetMouseButton(MouseCode button)
	{
		if (!s_Window) return false;
		return glfwGetMouseButton(s_Window, button) == GLFW_PRESS;
	}

	void RawInput::GetMousePosition(double& x, double& y)
	{
		if (!s_Window) return;
		glfwGetCursorPos(s_Window, &x, &y);
	}
}

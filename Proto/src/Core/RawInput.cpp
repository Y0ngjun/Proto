#include <stdexcept>
#include <GLFW/glfw3.h>
#include "RawInput.h"

namespace Proto
{
	GLFWwindow* RawInput::s_Window = nullptr;

	void RawInput::Initialize(GLFWwindow* window)
	{
		s_Window = window;
	}

	bool RawInput::GetKey(KeyCode key)
	{
		if (!s_Window)
		{
			throw std::runtime_error("Input system is not initialized.");
		}

		return glfwGetKey(s_Window, key) == GLFW_PRESS;
	}

	bool RawInput::GetMouseButton(MouseCode button)
	{
		if (!s_Window)
		{
			throw std::runtime_error("Input system is not initialized.");
		}

		return glfwGetMouseButton(s_Window, button) == GLFW_PRESS;
	}

	void RawInput::GetMousePosition(double& x, double& y)
	{
		if (!s_Window)
		{
			throw std::runtime_error("Input system is not initialized.");
		}

		glfwGetCursorPos(s_Window, &x, &y);
	}
}

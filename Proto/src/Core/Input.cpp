#include <stdexcept>

#include <GLFW/glfw3.h>

#include "Input.h"

namespace Proto
{
	GLFWwindow* Input::s_Window = nullptr;

	void Input::Initialize(GLFWwindow* window)
	{
		s_Window = window;
	}

	bool Input::GetKey(int key)
	{
		if (!s_Window)
		{
			throw std::runtime_error("Input system is not initialized.");
		}

		return glfwGetKey(s_Window, key) == GLFW_PRESS;
	}

	bool Input::GetMouseButton(int button)
	{
		if (!s_Window)
		{
			throw std::runtime_error("Input system is not initialized.");
		}

		return glfwGetMouseButton(s_Window, button) == GLFW_PRESS;
	}

	void Input::GetMousePosition(double& x, double& y)
	{
		if (!s_Window)
		{
			throw std::runtime_error("Input system is not initialized.");
		}

		glfwGetCursorPos(s_Window, &x, &y);
	}
}

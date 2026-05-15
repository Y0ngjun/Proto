/*
 * GLFW와 직접 통신하여 저수준 입력을 처리하는 클래스입니다.
 * 모든 입력 상태(키보드, 마우스)를 매 프레임 업데이트하고 관리합니다.
 */

#include <stdexcept>
#include <GLFW/glfw3.h>
#include <cstring>

#include "RawInput.h"

namespace Proto
{
	GLFWwindow* RawInput::m_Window = nullptr;
	bool RawInput::m_KeyStateBuffer[KEY_COUNT];
	bool RawInput::m_PreviousKeyStateBuffer[KEY_COUNT];

	void RawInput::Initialize(GLFWwindow* window)
	{
		m_Window = window;
		std::memset(m_KeyStateBuffer, 0, sizeof(m_KeyStateBuffer));
		std::memset(m_PreviousKeyStateBuffer, 0, sizeof(m_PreviousKeyStateBuffer));
	}

	void RawInput::Update()
	{
		if (!m_Window)
		{
			return;
		}

		// 이전 프레임 상태 보관
		std::memcpy(m_PreviousKeyStateBuffer, m_KeyStateBuffer, sizeof(m_KeyStateBuffer));

		// 모든 키 상태 폴링 (0 ~ KEY_COUNT-1 범위)
		for (int i = 0; i < KEY_COUNT; i++)
		{
			m_KeyStateBuffer[i] = glfwGetKey(m_Window, i) == GLFW_PRESS;
		}
	}

	bool RawInput::GetKey(KeyCode key)
	{
		if (key >= KEY_COUNT)
		{
			return false;
		}

		return m_KeyStateBuffer[key];
	}

	bool RawInput::GetKeyDown(KeyCode key)
	{
		if (key >= KEY_COUNT)
		{
			return false;
		}

		return m_KeyStateBuffer[key] && !m_PreviousKeyStateBuffer[key];
	}

	bool RawInput::GetMouseButton(MouseCode button)
	{
		if (!m_Window)
		{
			return false;
		}

		return glfwGetMouseButton(m_Window, button) == GLFW_PRESS;
	}

	void RawInput::GetMousePosition(double& x, double& y)
	{
		if (!m_Window)
		{
			return;
		}

		glfwGetCursorPos(m_Window, &x, &y);
	}
}

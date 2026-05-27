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
	bool RawInput::m_MouseButtonStateBuffer[MOUSE_BUTTON_COUNT];
	bool RawInput::m_PreviousMouseButtonStateBuffer[MOUSE_BUTTON_COUNT];
	double RawInput::m_MouseX = 0.0;
	double RawInput::m_MouseY = 0.0;
	double RawInput::m_PreviousMouseX = 0.0;
	double RawInput::m_PreviousMouseY = 0.0;

	void RawInput::Initialize(GLFWwindow* window)
	{
		m_Window = window;
		std::memset(m_KeyStateBuffer, 0, sizeof(m_KeyStateBuffer));
		std::memset(m_PreviousKeyStateBuffer, 0, sizeof(m_PreviousKeyStateBuffer));
		std::memset(m_MouseButtonStateBuffer, 0, sizeof(m_MouseButtonStateBuffer));
		std::memset(m_PreviousMouseButtonStateBuffer, 0, sizeof(m_PreviousMouseButtonStateBuffer));
		glfwGetCursorPos(m_Window, &m_MouseX, &m_MouseY);
		m_PreviousMouseX = m_MouseX;
		m_PreviousMouseY = m_MouseY;
	}

	void RawInput::Update()
	{
		if (!m_Window)
		{
			return;
		}

		// 이전 프레임 상태 보관
		std::memcpy(m_PreviousKeyStateBuffer, m_KeyStateBuffer, sizeof(m_KeyStateBuffer));
		std::memcpy(m_PreviousMouseButtonStateBuffer, m_MouseButtonStateBuffer, sizeof(m_MouseButtonStateBuffer));
		m_PreviousMouseX = m_MouseX;
		m_PreviousMouseY = m_MouseY;

		// 모든 키 상태 폴링 (0 ~ KEY_COUNT-1 범위)
		for (int i = 0; i < KEY_COUNT; i++)
		{
			m_KeyStateBuffer[i] = glfwGetKey(m_Window, i) == GLFW_PRESS;
		}

		// 마우스 버튼 상태 폴링
		for (int i = 0; i < MOUSE_BUTTON_COUNT; i++)
		{
			m_MouseButtonStateBuffer[i] = glfwGetMouseButton(m_Window, i) == GLFW_PRESS;
		}

		// 마우스 위치 폴링
		glfwGetCursorPos(m_Window, &m_MouseX, &m_MouseY);
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

	bool RawInput::GetKeyUp(KeyCode key)
	{
		if (key >= KEY_COUNT)
		{
			return false;
		}

		return !m_KeyStateBuffer[key] && m_PreviousKeyStateBuffer[key];
	}

	bool RawInput::GetMouseButton(MouseCode button)
	{
		if (button >= MOUSE_BUTTON_COUNT)
		{
			return false;
		}

		return m_MouseButtonStateBuffer[button];
	}

	bool RawInput::GetMouseButtonDown(MouseCode button)
	{
		if (button >= MOUSE_BUTTON_COUNT)
		{
			return false;
		}

		return m_MouseButtonStateBuffer[button] && !m_PreviousMouseButtonStateBuffer[button];
	}

	bool RawInput::GetMouseButtonUp(MouseCode button)
	{
		if (button >= MOUSE_BUTTON_COUNT)
		{
			return false;
		}

		return !m_MouseButtonStateBuffer[button] && m_PreviousMouseButtonStateBuffer[button];
	}

	void RawInput::GetMousePosition(double& x, double& y)
	{
		x = m_MouseX;
		y = m_MouseY;
	}

	void RawInput::GetMouseDelta(double& dx, double& dy)
	{
		dx = m_MouseX - m_PreviousMouseX;
		dy = m_MouseY - m_PreviousMouseY;
	}
}

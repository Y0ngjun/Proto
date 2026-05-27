/*
 * GLFW와 직접 통신하여 저수준 입력을 처리하는 클래스입니다.
 * 모든 입력 상태(키보드, 마우스)를 매 프레임 업데이트하고 관리합니다.
 */

#pragma once

#include "KeyCodes.h"
#include "MouseCodes.h"

struct GLFWwindow;

namespace Proto
{
	class RawInput
	{
	public:
		static void Initialize(GLFWwindow* window);
		static void Update();

		static bool GetKey(KeyCode key);
		static bool GetKeyDown(KeyCode key);
		static bool GetKeyUp(KeyCode key);
		static bool GetMouseButton(MouseCode button);
		static bool GetMouseButtonDown(MouseCode button);
		static bool GetMouseButtonUp(MouseCode button);
		static void GetMousePosition(double& x, double& y);
		static void GetMouseDelta(double& dx, double& dy);

	private:
		static GLFWwindow* m_Window;

		static constexpr int KEY_COUNT = 512;
		static bool m_KeyStateBuffer[KEY_COUNT];
		static bool m_PreviousKeyStateBuffer[KEY_COUNT];

		static constexpr int MOUSE_BUTTON_COUNT = 8;
		static bool m_MouseButtonStateBuffer[MOUSE_BUTTON_COUNT];
		static bool m_PreviousMouseButtonStateBuffer[MOUSE_BUTTON_COUNT];

		static double m_MouseX;
		static double m_MouseY;
		static double m_PreviousMouseX;
		static double m_PreviousMouseY;
	};
}

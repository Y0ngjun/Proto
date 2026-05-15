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
		static bool GetMouseButton(MouseCode button);
		static void GetMousePosition(double& x, double& y);

	private:
		static GLFWwindow* m_Window;

		static constexpr int KEY_COUNT = 512;
		static bool m_KeyStateBuffer[KEY_COUNT];
		static bool m_PreviousKeyStateBuffer[KEY_COUNT];
	};
}

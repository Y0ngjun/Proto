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

		// 현재 상태 업데이트 (자주 쓰이는 키 범위만 루프 돌거나 필요할 때 체크)
		// 여기서는 성능을 위해 GetKey/GetKeyDown 호출 시점에 업데이트하는 대신
		// 전체 상태를 한 번에 동기화할 수도 있지만, GLFW는 GetKey 호출 방식이므로 
		// GetKeyDown 호출 시점에 이전 상태와 비교하는 방식이 더 적합할 수 있습니다.
		// 하지만 매 프레임 모든 키를 긁어오는 것은 오버헤드이므로, 
		// 여기서는 수동 상태 동기화 대신 '사용자 정의 상태 맵' 방식으로 전환합니다.
	}

	bool RawInput::GetKey(KeyCode key)
	{
		if (!s_Window) return false;
		bool current = glfwGetKey(s_Window, key) == GLFW_PRESS;
		s_KeyStateBuffer[key] = current; // 현재 상태 기록
		return current;
	}

	bool RawInput::GetKeyDown(KeyCode key)
	{
		if (!s_Window) return false;
		
		bool current = glfwGetKey(s_Window, key) == GLFW_PRESS;
		bool previous = s_PreviousKeyStateBuffer[key];
		
		s_KeyStateBuffer[key] = current; // 현재 상태 기록

		return current && !previous;
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

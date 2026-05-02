#include "Input.h"
#include "RawInput.h"
#include "Application.h"

namespace Proto
{
	bool Input::GetKey(KeyCode key)
	{
		// 시스템 레벨에서 게임 뷰 포커스 여부 확인
		if (!Application::Get().IsGameViewFocused())
			return false;

		return RawInput::GetKey(key);
	}

	bool Input::GetMouseButton(MouseCode button)
	{
		if (!Application::Get().IsGameViewFocused())
			return false;

		return RawInput::GetMouseButton(button);
	}

	void Input::GetMousePosition(double& x, double& y)
	{
		RawInput::GetMousePosition(x, y);
	}
}

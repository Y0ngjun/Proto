/*
 * 게임 로직에서 사용하는 입력 시스템 클래스입니다.
 * 에디터 상태(게임 뷰 포커스 등)에 따라 입력 활성화 여부를 제어합니다.
 */

#include "Input.h"
#include "RawInput.h"

namespace Proto
{
	bool Input::m_Enabled = true;

	bool Input::GetKey(KeyCode key)
	{
		if (!m_Enabled)
		{
			return false;
		}

		return RawInput::GetKey(key);
	}

	bool Input::GetMouseButton(MouseCode button)
	{
		if (!m_Enabled)
		{
			return false;
		}

		return RawInput::GetMouseButton(button);
	}

	void Input::GetMousePosition(double& x, double& y)
	{
		RawInput::GetMousePosition(x, y);
	}
}

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

	bool Input::GetKeyDown(KeyCode key)
	{
		if (!m_Enabled)
		{
			return false;
		}

		return RawInput::GetKeyDown(key);
	}

	bool Input::GetKeyUp(KeyCode key)
	{
		if (!m_Enabled)
		{
			return false;
		}

		return RawInput::GetKeyUp(key);
	}

	bool Input::GetMouseButton(MouseCode button)
	{
		if (!m_Enabled)
		{
			return false;
		}

		return RawInput::GetMouseButton(button);
	}

	bool Input::GetMouseButtonDown(MouseCode button)
	{
		if (!m_Enabled)
		{
			return false;
		}

		return RawInput::GetMouseButtonDown(button);
	}

	bool Input::GetMouseButtonUp(MouseCode button)
	{
		if (!m_Enabled)
		{
			return false;
		}

		return RawInput::GetMouseButtonUp(button);
	}

	void Input::GetMousePosition(double& x, double& y)
	{
		RawInput::GetMousePosition(x, y);
	}

	void Input::GetMouseDelta(double& dx, double& dy)
	{
		if (!m_Enabled)
		{
			dx = 0.0;
			dy = 0.0;
			return;
		}

		RawInput::GetMouseDelta(dx, dy);
	}
}

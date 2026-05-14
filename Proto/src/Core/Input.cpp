#include "Input.h"
#include "RawInput.h"

namespace Proto
{
	bool Input::s_Enabled = true;

	bool Input::GetKey(KeyCode key)
	{
		if (!s_Enabled)
			return false;

		return RawInput::GetKey(key);
	}

	bool Input::GetMouseButton(MouseCode button)
	{
		if (!s_Enabled)
			return false;

		return RawInput::GetMouseButton(button);
	}

	void Input::GetMousePosition(double& x, double& y)
	{
		RawInput::GetMousePosition(x, y);
	}
}

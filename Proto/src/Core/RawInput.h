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
		static GLFWwindow* s_Window;
	};
}

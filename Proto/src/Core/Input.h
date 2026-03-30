#pragma once

struct GLFWwindow;

namespace Proto
{
	class Input
	{
	public:
		static void Initialize(GLFWwindow* window);

		static bool GetKey(int key);
		static bool GetMouseButton(int button);
		static void GetMousePosition(double& x, double& y);

	private:
		static GLFWwindow* s_Window;
	};
}

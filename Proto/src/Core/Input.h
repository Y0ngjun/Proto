#pragma once

#include "KeyCodes.h"
#include "MouseCodes.h"

namespace Proto
{
	class Input
	{
	public:
		// 게임 뷰가 포커스된 상태에서만 키 입력을 반환합니다.
		static bool GetKey(KeyCode key);
		
		// 게임 뷰가 포커스된 상태에서만 마우스 버튼 입력을 반환합니다.
		static bool GetMouseButton(MouseCode button);
		
		// 마우스 위치를 가져옵니다.
		static void GetMousePosition(double& x, double& y);

		static void SetEnabled(bool enabled) { s_Enabled = enabled; }
		static bool IsEnabled() { return s_Enabled; }

	private:
		static bool s_Enabled;
	};
}

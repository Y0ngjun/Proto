/*
 * 게임 로직에서 사용하는 입력 시스템 클래스입니다.
 * 에디터 상태(게임 뷰 포커스 등)에 따라 입력 활성화 여부를 제어합니다.
 */

#pragma once

#include "KeyCodes.h"
#include "MouseCodes.h"

namespace Proto
{
	class Input
	{
	public:
		// 키가 눌려있는지 확인합니다. (게임 뷰가 활성화된 경우에만 유효)
		static bool GetKey(KeyCode key);

		// 마우스 버튼이 눌려있는지 확인합니다. (게임 뷰가 활성화된 경우에만 유효)
		static bool GetMouseButton(MouseCode button);

		// 현재 마우스 위치를 가져옵니다.
		static void GetMousePosition(double& x, double& y);

		static void SetEnabled(bool enabled)
		{
			m_Enabled = enabled;
		}

		static bool IsEnabled()
		{
			return m_Enabled;
		}

	private:
		static bool m_Enabled;
	};
}

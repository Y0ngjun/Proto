/*
 * 엔진에서 사용하는 마우스 버튼 코드를 정의합니다.
 * GLFW 마우스 코드를 기반으로 매핑되어 있습니다.
 */

#pragma once

#include <stdint.h>

namespace Proto
{
	using MouseCode = uint16_t;

	namespace Mouse
	{
		enum : MouseCode
		{
			BUTTON_0             = 0,
			BUTTON_1             = 1,
			BUTTON_2             = 2,
			BUTTON_3             = 3,
			BUTTON_4             = 4,
			BUTTON_5             = 5,
			BUTTON_6             = 6,
			BUTTON_7             = 7,

			BUTTON_LEFT          = BUTTON_0,
			BUTTON_RIGHT         = BUTTON_1,
			BUTTON_MIDDLE        = BUTTON_2,
			BUTTON_LAST          = BUTTON_7
		};
	}
}

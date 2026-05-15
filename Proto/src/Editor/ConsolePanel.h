/*
 * 엔진의 로그 메시지를 표시하는 에디터 콘솔 패널 클래스입니다.
 * 로그 레벨별 색상 구분, 자동 스크롤, 상세 내용 보기 기능을 제공합니다.
 */

#pragma once

#include <vector>
#include <string>

namespace Proto
{
	class ConsolePanel
	{
	public:
		ConsolePanel();
		~ConsolePanel() = default;

		void OnImGuiRender();

	private:
		bool m_AutoScroll = true;
		int m_SelectedMessageIndex = -1;
	};
}

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

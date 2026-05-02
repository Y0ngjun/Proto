#include "ConsolePanel.h"
#include "../Core/Log.h"
#include <imgui.h>

namespace Proto
{
	ConsolePanel::ConsolePanel()
	{
	}

	void ConsolePanel::OnImGuiRender()
	{
		ImGui::Begin("Console");

		// 1. Toolbar 영역
		if (ImGui::Button("Clear"))
		{
			Log::Clear();
			m_SelectedMessageIndex = -1;
		}

		ImGui::SameLine();
		ImGui::Checkbox("Auto-scroll", &m_AutoScroll);

		ImGui::Separator();

		// 2. 로그 리스트 영역 (상단 70%)
		float footerHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing() * 3.0f; // 대략적인 하단 높이 예약
		ImGui::BeginChild("LogList", ImVec2(0, -footerHeight), false, ImGuiWindowFlags_HorizontalScrollbar);

		const auto& messages = Log::GetMessages();
		for (int i = 0; i < (int)messages.size(); i++)
		{
			const auto& msg = messages[i];
			ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
			
			if (msg.Level == LogLevel::Warn) color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
			else if (msg.Level == LogLevel::Error) color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
			else if (msg.Level == LogLevel::Info) color = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);

			std::string label = "[" + msg.Timestamp + "] " + msg.Message;
			
			if (ImGui::Selectable((label + "##" + std::to_string(i)).c_str(), m_SelectedMessageIndex == i))
			{
				m_SelectedMessageIndex = i;
			}
			
			if (m_SelectedMessageIndex == i)
			{
				// 선택된 항목은 색상 강조 (필요 시)
			}
		}

		if (m_AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			ImGui::SetScrollHereY(1.0f);

		ImGui::EndChild();

		ImGui::Separator();

		// 3. 디테일 영역 (하단 30%)
		ImGui::BeginChild("LogDetail", ImVec2(0, 0), true);
		if (m_SelectedMessageIndex >= 0 && m_SelectedMessageIndex < (int)messages.size())
		{
			const auto& selectedMsg = messages[m_SelectedMessageIndex];
			ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
			if (selectedMsg.Level == LogLevel::Warn) color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
			else if (selectedMsg.Level == LogLevel::Error) color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
			
			ImGui::PushStyleColor(ImGuiCol_Text, color);
			ImGui::TextWrapped("%s", selectedMsg.Message.c_str());
			ImGui::PopStyleColor();
		}
		ImGui::EndChild();

		ImGui::End();
	}
}

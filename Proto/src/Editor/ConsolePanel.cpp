#include "ConsolePanel.h"
#include "../Core/Log.h"
#include <imgui.h>

namespace Proto
{
	ConsolePanel::ConsolePanel()
	{
	}

	static ImVec4 GetLevelColor(LogLevel level)
	{
		switch (level)
		{
		case LogLevel::Info:  return ImVec4(0.0f, 1.0f, 1.0f, 1.0f); // Cyan
		case LogLevel::Warn:  return ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
		case LogLevel::Error: return ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
		}
		return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // White
	}

	void ConsolePanel::OnImGuiRender()
	{
		ImGui::Begin("Console");

		if (ImGui::Button("Clear"))
		{
			Log::Clear();
			m_SelectedMessageIndex = -1;
		}

		ImGui::SameLine();
		ImGui::Checkbox("Auto-scroll", &m_AutoScroll);

		ImGui::Separator();

		const float footerHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing() * 3.0f;
		ImGui::BeginChild("LogList", ImVec2(0, -footerHeight), false, ImGuiWindowFlags_HorizontalScrollbar);

		const auto& messages = Log::GetMessages();
		for (int i = 0; i < (int)messages.size(); i++)
		{
			const auto& msg = messages[i];
			const std::string label = "[" + msg.Timestamp + "] " + msg.Message;
			
			ImGui::PushStyleColor(ImGuiCol_Text, GetLevelColor(msg.Level));
			if (ImGui::Selectable((label + "##" + std::to_string(i)).c_str(), m_SelectedMessageIndex == i))
			{
				m_SelectedMessageIndex = i;
			}
			ImGui::PopStyleColor();
		}

		if (m_AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			ImGui::SetScrollHereY(1.0f);

		ImGui::EndChild();

		ImGui::Separator();

		ImGui::BeginChild("LogDetail", ImVec2(0, 0), true);
		if (m_SelectedMessageIndex >= 0 && m_SelectedMessageIndex < (int)messages.size())
		{
			const auto& selectedMsg = messages[m_SelectedMessageIndex];
			ImGui::PushStyleColor(ImGuiCol_Text, GetLevelColor(selectedMsg.Level));
			ImGui::TextWrapped("%s", selectedMsg.Message.c_str());
			ImGui::PopStyleColor();
		}
		ImGui::EndChild();

		ImGui::End();
	}
}

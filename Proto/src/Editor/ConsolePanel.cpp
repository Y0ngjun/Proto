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

		if (ImGui::Button("Clear"))
		{
			Log::Clear();
		}

		ImGui::SameLine();
		ImGui::Checkbox("Auto-scroll", &m_AutoScroll);

		ImGui::Separator();

		ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

		const auto& messages = Log::GetMessages();
		for (const auto& msg : messages)
		{
			ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // Default White
			
			if (msg.Level == LogLevel::Warn) color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);      // Yellow
			else if (msg.Level == LogLevel::Error) color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
			else if (msg.Level == LogLevel::Info) color = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);  // Cyan

			ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "[%s]", msg.Timestamp.c_str());
			ImGui::SameLine();
			ImGui::TextColored(color, "%s", msg.Message.c_str());
		}

		if (m_AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			ImGui::SetScrollHereY(1.0f);

		ImGui::EndChild();
		ImGui::End();
	}
}

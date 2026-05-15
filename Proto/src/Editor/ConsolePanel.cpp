/*
 * 엔진의 로그 메시지를 표시하는 에디터 콘솔 패널 클래스입니다.
 * 로그 레벨별 색상 구분, 자동 스크롤, 상세 내용 보기 기능을 제공합니다.
 */

#include "ConsolePanel.h"
#include "../Core/Log.h"
#include <imgui.h>

namespace Proto
{
	namespace
	{
		static constexpr ImVec4 COLOR_INFO = ImVec4(0.0f, 1.0f, 1.0f, 1.0f); // Cyan
		static constexpr ImVec4 COLOR_WARN = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
		static constexpr ImVec4 COLOR_ERROR = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
		static constexpr ImVec4 COLOR_WHITE = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

		static constexpr float FOOTER_HEIGHT_MULTIPLIER = 3.0f;
	}

	ConsolePanel::ConsolePanel()
	{}

	static ImVec4 GetLevelColor(LogLevel level)
	{
		switch (level)
		{
		case LogLevel::Info:  return COLOR_INFO;
		case LogLevel::Warn:  return COLOR_WARN;
		case LogLevel::Error: return COLOR_ERROR;
		}

		return COLOR_WHITE;
	}

	void ConsolePanel::OnImGuiRender()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
		ImGui::Begin("Console");

		// 헤더 바: 통일된 스타일
		const float barHeight = ImGui::GetFrameHeight() + 8.0f;
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
		ImGui::BeginChild("ConsoleHeader", ImVec2(0, barHeight), false,
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		ImGui::SetCursorPos(ImVec2(8.0f, 4.0f));

		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
		if (ImGui::Button("Clear", ImVec2(120.0f, 0)))
		{
			Log::Clear();
			m_SelectedMessageIndex = -1;
		}
		ImGui::PopStyleVar();

		ImGui::SameLine(0, 8);
		ImGui::Checkbox("Auto-scroll", &m_AutoScroll);

		ImGui::EndChild();
		ImGui::PopStyleColor();
		ImGui::Separator();

		const float footerHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing() * FOOTER_HEIGHT_MULTIPLIER;
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
		{
			ImGui::SetScrollHereY(1.0f);
		}

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
		ImGui::PopStyleVar(2);
	}
}

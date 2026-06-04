/*
 * 엔진의 로그 메시지를 표시하는 에디터 콘솔 패널 클래스입니다.
 * 로그 레벨별 색상 구분, 자동 스크롤, 상세 내용 보기 기능을 제공합니다.
 */

#include "ConsolePanel.h"
#include "EditorStyle.h"
#include "../Core/Log.h"
#include <imgui.h>

namespace Proto
{
	namespace
	{
		static constexpr float FOOTER_HEIGHT_MULTIPLIER = 6.0f;
	}

	ConsolePanel::ConsolePanel()
	{}

	static ImVec4 GetLevelColor(LogLevel level)
	{
		switch (level)
		{
		case LogLevel::Info:  return EditorStyle::COLOR_LOG_INFO;
		case LogLevel::Warn:  return EditorStyle::COLOR_LOG_WARN;
		case LogLevel::Error: return EditorStyle::COLOR_LOG_ERROR;
		}

		return EditorStyle::COLOR_LOG_DEFAULT;
	}

	void ConsolePanel::OnImGuiRender()
	{

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_Text, EditorStyle::COLOR_TITLE_TEXT);
		ImGui::Begin("Console");
		ImGui::PopStyleColor();

		// 헤더 바: 통일된 스타일
		const float barHeight = ImGui::GetFrameHeight() + 8.0f;
		ImGui::PushStyleColor(ImGuiCol_ChildBg, EditorStyle::COLOR_PANEL_HEADER_BG);
		ImGui::BeginChild("ConsoleHeader", ImVec2(0, barHeight), false,
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		ImGui::SetCursorPos(ImVec2(8.0f, 4.0f));

		// Clear 버튼 (맨 왼쪽)
		ImGui::PushStyleColor(ImGuiCol_Button,        EditorStyle::COLOR_GIZMO_BTN);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EditorStyle::COLOR_GIZMO_BTN_HOVERED);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,  EditorStyle::COLOR_GIZMO_BTN_ACTIVE);
		ImGui::PushStyleColor(ImGuiCol_Text,          EditorStyle::COLOR_GIZMO_BTN_TEXT);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
		if (ImGui::Button("Clear", ImVec2(120.0f, 0)))
		{
			Log::Clear();
			m_SelectedMessageIndex = -1;
		}
		ImGui::PopStyleVar();
		ImGui::PopStyleColor(4);

		// 필터 버튼 (오른쪽 정렬) — Log / Warning / Error
		const float filterBtnWidth = 120.0f;
		const float filterSpacing  = 4.0f;
		const float totalRight     = filterBtnWidth * 3 + filterSpacing * 2 + 8.0f;
		ImGui::SameLine(ImGui::GetContentRegionAvail().x - totalRight + ImGui::GetCursorPosX() - ImGui::GetScrollX());

		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);

		auto PushFilterBtnStyle = [&](bool active)
		{
			if (active)
			{
				ImGui::PushStyleColor(ImGuiCol_Button,        EditorStyle::COLOR_CONSOLE_FILTER_ACTIVE);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EditorStyle::COLOR_CONSOLE_FILTER_ACTIVE_HOVERED);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive,  EditorStyle::COLOR_CONSOLE_FILTER_ACTIVE_PRESSED);
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Button,        EditorStyle::COLOR_GIZMO_BTN);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EditorStyle::COLOR_GIZMO_BTN_HOVERED);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive,  EditorStyle::COLOR_GIZMO_BTN_ACTIVE);
			}
			ImGui::PushStyleColor(ImGuiCol_Text, EditorStyle::COLOR_GIZMO_BTN_TEXT);
		};

		PushFilterBtnStyle(m_FilterInfo);
		if (ImGui::Button("Log", ImVec2(filterBtnWidth, 0)))
			m_FilterInfo = !m_FilterInfo;
		ImGui::PopStyleColor(4);

		ImGui::SameLine(0, filterSpacing);

		PushFilterBtnStyle(m_FilterWarn);
		if (ImGui::Button("Warning", ImVec2(filterBtnWidth, 0)))
			m_FilterWarn = !m_FilterWarn;
		ImGui::PopStyleColor(4);

		ImGui::SameLine(0, filterSpacing);

		PushFilterBtnStyle(m_FilterError);
		if (ImGui::Button("Error", ImVec2(filterBtnWidth, 0)))
			m_FilterError = !m_FilterError;
		ImGui::PopStyleColor(4);

		ImGui::PopStyleVar();

		ImGui::EndChild();
		ImGui::PopStyleColor();
		ImGui::Separator();

		const float footerHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing() * FOOTER_HEIGHT_MULTIPLIER;
		ImGui::BeginChild("LogList", ImVec2(0, -footerHeight), false);

		const auto& messages = Log::GetMessages();
		for (int i = 0; i < (int)messages.size(); i++)
		{
			const auto& msg = messages[i];

			if (msg.Level == LogLevel::Info  && !m_FilterInfo)  continue;
			if (msg.Level == LogLevel::Warn  && !m_FilterWarn)  continue;
			if (msg.Level == LogLevel::Error && !m_FilterError) continue;

			const std::string label = "[" + msg.Timestamp + "] " + msg.Message;

			ImGui::PushStyleColor(ImGuiCol_Text, GetLevelColor(msg.Level));
			if (ImGui::Selectable((label + "##" + std::to_string(i)).c_str(), m_SelectedMessageIndex == i))
			{
				m_SelectedMessageIndex = i;
			}
			ImGui::PopStyleColor();
		}

		if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
		{
			ImGui::SetScrollHereY(1.0f);
		}

		ImGui::EndChild();

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

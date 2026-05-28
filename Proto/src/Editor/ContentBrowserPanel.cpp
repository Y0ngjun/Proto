/*
 * 프로젝트 자산을 탐색하고 관리하는 에디터 콘텐츠 브라우저 패널 클래스입니다.
 * 디렉토리 탐색, 파일 실행(프로젝트/씬 로드) 기능을 제공합니다.
 */

#include "ContentBrowserPanel.h"
#include "EditorStyle.h"
#include "../Core/Application.h"
#include "../Core/Project.h"
#include <imgui.h>

namespace Proto
{
	namespace
	{
		static constexpr float DEFAULT_PADDING = 16.0f;
		static constexpr float DEFAULT_CELL_SIZE = 100.0f;
	}

	ContentBrowserPanel::ContentBrowserPanel()
	{}

	static ImVec4 GetFileColor(const std::filesystem::directory_entry& entry)
	{
		if (entry.is_directory())
		{
			return EditorStyle::COLOR_FILE_FOLDER;
		}

		if (entry.path().extension() == ".proto")
		{
			return EditorStyle::COLOR_FILE_PROJECT;
		}

		return EditorStyle::COLOR_FILE_DEFAULT;
	}

	static const char* GetFileIcon(const std::filesystem::directory_entry& entry)
	{
		if (entry.is_directory())
		{
			return "DIR";
		}

		if (entry.path().extension() == ".proto")
		{
			return "PROJ";
		}

		return "FILE";
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_Text, EditorStyle::COLOR_TITLE_TEXT);
		ImGui::Begin("Content Browser");
		ImGui::PopStyleColor();

		const std::filesystem::path projectPath = Project::GetProjectDirectory();
		if (projectPath.empty())
		{
			ImGui::Text("No Project Open.");
			ImGui::End();
			ImGui::PopStyleVar(2);
			return;
		}

		// 프로젝트가 변경되었거나 디렉토리가 유효하지 않으면 루트로 이동
		if (projectPath != m_BaseDirectory || m_CurrentDirectory.empty() || !std::filesystem::exists(m_CurrentDirectory))
		{
			m_BaseDirectory = projectPath;
			m_CurrentDirectory = projectPath;
		}

		// 헤더 바: 통일된 스타일
		const float barHeight = ImGui::GetFrameHeight() + 8.0f;
		ImGui::PushStyleColor(ImGuiCol_ChildBg, EditorStyle::COLOR_PANEL_HEADER_BG);
		ImGui::BeginChild("ContentBrowserHeader", ImVec2(0, barHeight), false,
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		ImGui::SetCursorPos(ImVec2(8.0f, 4.0f));

		// 버튼 영역: 항상 자리를 차지하도록 구성 (루트면 비활성화)
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
		const bool isRoot = (m_CurrentDirectory == projectPath);
		
		if (isRoot) ImGui::BeginDisabled();

		if (ImGui::Button("Root", ImVec2(120.0f, 0)))
		{
			m_CurrentDirectory = projectPath;
		}

		ImGui::SameLine(0, 4);

		if (ImGui::Button("<- Back", ImVec2(120.0f, 0)))
		{
			m_CurrentDirectory = m_CurrentDirectory.parent_path();
		}

		if (isRoot) ImGui::EndDisabled();
		ImGui::PopStyleVar();

		// 경로 텍스트: 버튼 뒤에 고정된 위치에 출력
		ImGui::SameLine(0, 8);
		std::string relativePath = std::filesystem::relative(m_CurrentDirectory, projectPath).string();
		
		std::string displayPath;
		if (relativePath == ".") 
		{
			displayPath = "Root";
		}
		else 
		{
			displayPath = "Root/" + relativePath;
			// 슬래시 스타일 통일 (윈도우 역슬래시 -> 포워드 슬래시)
			std::replace(displayPath.begin(), displayPath.end(), '\\', '/');
		}
		
		ImGui::Text("%s", displayPath.c_str());

		ImGui::EndChild();
		ImGui::PopStyleColor();
		ImGui::Separator();

		const float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / (DEFAULT_CELL_SIZE + DEFAULT_PADDING));
		if (columnCount < 1)
		{
			columnCount = 1;
		}

		ImGui::Columns(columnCount, 0, false);

		try
		{
			for (const auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
			{
				const auto& path = directoryEntry.path();
				const std::string filenameString = path.filename().string();

				ImGui::PushID(filenameString.c_str());
				ImGui::PushStyleColor(ImGuiCol_Button, GetFileColor(directoryEntry));

				ImGui::Button(GetFileIcon(directoryEntry), ImVec2(DEFAULT_CELL_SIZE, DEFAULT_CELL_SIZE));
				ImGui::PopStyleColor();

				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					if (directoryEntry.is_directory())
					{
						m_CurrentDirectory /= path.filename();
					}
					else if (path.extension() == ".proto")
					{
						Application::Get().OpenProject(path);
					}
					else if (path.extension() == ".scene")
					{
						Application::Get().OpenScene(path);
					}
				}

				ImGui::TextWrapped("%s", filenameString.c_str());
				ImGui::NextColumn();
				ImGui::PopID();
			}
		}
		catch (...)
		{
			ImGui::Text("Error reading directory.");
		}

		ImGui::Columns(1);
		ImGui::End();
		ImGui::PopStyleVar(2);
	}
}

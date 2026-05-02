#include "ContentBrowserPanel.h"
#include "../Core/Application.h"
#include "../Core/Project.h"
#include <imgui.h>
#include <iostream>

namespace Proto
{
	ContentBrowserPanel::ContentBrowserPanel()
	{
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		std::filesystem::path projectPath = Project::GetProjectDirectory();
		
		if (projectPath.empty())
		{
			ImGui::Begin("Content Browser");
			ImGui::Text("No Project Open.");
			ImGui::End();
			return;
		}

		if (m_CurrentDirectory.empty() || !std::filesystem::exists(m_CurrentDirectory))
		{
			m_CurrentDirectory = projectPath;
		}

		ImGui::Begin("Content Browser");

		if (m_CurrentDirectory != projectPath)
		{
			if (ImGui::Button("Root"))
			{
				m_CurrentDirectory = projectPath;
			}
			ImGui::SameLine();
			if (ImGui::Button(".. Back"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
			ImGui::SameLine();
		}
		
		ImGui::Text("Path: %s", std::filesystem::relative(m_CurrentDirectory, projectPath).string().c_str());
		ImGui::Separator();

		static float padding = 16.0f;
		static float cellSize = 100.0f;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / (cellSize + padding));
		if (columnCount < 1) columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		try 
		{
			for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
			{
				const auto& path = directoryEntry.path();
				std::string filenameString = path.filename().string();
				bool isDirectory = directoryEntry.is_directory();

				ImGui::PushID(filenameString.c_str());
				// 색상 및 아이콘 구분
				if (isDirectory)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.8f, 1.0f)); // Folder Blue
				}
				else if (path.extension() == ".proto")
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.5f, 0.2f, 1.0f)); // Project Orange
				}
				else
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f)); // File Grey
				}

				// 싱글 클릭 버튼 (이동 로직 없음)
				ImGui::Button(isDirectory ? "DIR" : (path.extension() == ".proto" ? "PROJ" : "FILE"), ImVec2(cellSize, cellSize));
				
				ImGui::PopStyleColor();

				// 더블 클릭 시 동작
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					if (isDirectory)
					{
						m_CurrentDirectory /= path.filename();
					}
					else if (path.extension() == ".proto")
					{
						// 프로젝트 파일 더블 클릭 시 프로젝트 로드
						Application::Get().OpenProject(path);
					}
					else if (path.extension() == ".scene")
					{
						// 씬 파일 더블 클릭 시 씬 로드
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
	}
}

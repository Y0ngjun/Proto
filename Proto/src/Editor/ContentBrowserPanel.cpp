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

	static ImVec4 GetFileColor(const std::filesystem::directory_entry& entry)
	{
		if (entry.is_directory()) return ImVec4(0.3f, 0.5f, 0.8f, 1.0f); // Folder Blue
		if (entry.path().extension() == ".proto") return ImVec4(0.8f, 0.5f, 0.2f, 1.0f); // Project Orange
		return ImVec4(0.2f, 0.2f, 0.2f, 1.0f); // File Grey
	}

	static const char* GetFileIcon(const std::filesystem::directory_entry& entry)
	{
		if (entry.is_directory()) return "DIR";
		if (entry.path().extension() == ".proto") return "PROJ";
		return "FILE";
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		const std::filesystem::path projectPath = Project::GetProjectDirectory();
		if (projectPath.empty())
		{
			ImGui::Text("No Project Open.");
			ImGui::End();
			return;
		}

		if (m_CurrentDirectory.empty() || !std::filesystem::exists(m_CurrentDirectory))
			m_CurrentDirectory = projectPath;

		if (m_CurrentDirectory != projectPath)
		{
			if (ImGui::Button("Root")) m_CurrentDirectory = projectPath;
			ImGui::SameLine();
			if (ImGui::Button(".. Back")) m_CurrentDirectory = m_CurrentDirectory.parent_path();
			ImGui::SameLine();
		}
		
		ImGui::Text("Path: %s", std::filesystem::relative(m_CurrentDirectory, projectPath).string().c_str());
		ImGui::Separator();

		const float padding = 16.0f;
		const float cellSize = 100.0f;
		const float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / (cellSize + padding));
		if (columnCount < 1) columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		try 
		{
			for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
			{
				const auto& path = directoryEntry.path();
				std::string filenameString = path.filename().string();

				ImGui::PushID(filenameString.c_str());
				ImGui::PushStyleColor(ImGuiCol_Button, GetFileColor(directoryEntry));
				
				ImGui::Button(GetFileIcon(directoryEntry), ImVec2(cellSize, cellSize));
				ImGui::PopStyleColor();

				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					if (directoryEntry.is_directory())
						m_CurrentDirectory /= path.filename();
					else if (path.extension() == ".proto")
						Application::Get().OpenProject(path);
					else if (path.extension() == ".scene")
						Application::Get().OpenScene(path);
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

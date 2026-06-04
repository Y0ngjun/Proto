/*
 * 프로젝트 자산을 탐색하고 관리하는 에디터 콘텐츠 브라우저 패널 클래스입니다.
 * 디렉토리 탐색, 파일 실행(프로젝트/씬 로드) 기능을 제공합니다.
 */

#include "ContentBrowserPanel.h"
#include "EditorStyle.h"
#include "../Core/Application.h"
#include "../Core/Project.h"
#include <imgui.h>
#include <fstream>

namespace Proto
{
	namespace
	{
		static constexpr float FOLDER_TREE_RATIO = 0.28f;
		static constexpr float LIST_PADDING      = 8.0f;
	}

	ContentBrowserPanel::ContentBrowserPanel()
	{}

	void ContentBrowserPanel::CreateAsset(const char* type)
	{
		if (std::string(type) == "Folder")
		{
			std::filesystem::path p = m_CurrentDirectory / "New Folder";
			int suffix = 1;
			while (std::filesystem::exists(p))
				p = m_CurrentDirectory / ("New Folder " + std::to_string(suffix++));
			std::filesystem::create_directory(p);
		}
		else if (std::string(type) == "Script")
		{
			std::filesystem::path p = m_CurrentDirectory / "NewScript.h";
			int suffix = 1;
			while (std::filesystem::exists(p))
				p = m_CurrentDirectory / ("NewScript" + std::to_string(suffix++) + ".h");
			std::ofstream f(p);
			f << "#pragma once\n#include \"../Core/NativeScript.h\"\n\nnamespace Proto\n{\n\tclass "
			  << p.stem().string() << " : public NativeScript\n\t{\n\t};\n}\n";
		}
		else if (std::string(type) == "Scene")
		{
			std::filesystem::path p = m_CurrentDirectory / "NewScene.proto";
			int suffix = 1;
			while (std::filesystem::exists(p))
				p = m_CurrentDirectory / ("NewScene" + std::to_string(suffix++) + ".proto");
			std::ofstream f(p);
		}
	}

	static const char* GetFileIcon(const std::filesystem::directory_entry& entry)
	{
		if (entry.is_directory())
			return "DIR";

		if (entry.path().extension() == ".proj")
			return "PROJ";

		if (entry.path().extension() == ".proto")
			return "SCENE";

		if (entry.path().extension() == ".h" || entry.path().extension() == ".cpp")
			return "SCRIPT";

		return "FILE";
	}

	// 왼쪽 패널: 폴더 트리 재귀 렌더링
	void ContentBrowserPanel::DrawFolderTree(const std::filesystem::path& dir, const std::filesystem::path& base)
	{
		try
		{
			for (const auto& entry : std::filesystem::directory_iterator(dir))
			{
				if (!entry.is_directory())
					continue;

				const std::string name = entry.path().filename().string();
				ImGui::PushID(entry.path().string().c_str());

				bool hasSubFolders = false;
				try
				{
					for (const auto& sub : std::filesystem::directory_iterator(entry.path()))
					{
						if (sub.is_directory()) { hasSubFolders = true; break; }
					}
				}
				catch (...) {}

				ImGuiTreeNodeFlags flags =
					ImGuiTreeNodeFlags_OpenOnArrow |
					ImGuiTreeNodeFlags_SpanAvailWidth;

				if (!hasSubFolders)
					flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

				if (m_CurrentDirectory == entry.path())
					flags |= ImGuiTreeNodeFlags_Selected;

				const bool opened = ImGui::TreeNodeEx(name.c_str(), flags);

				if (ImGui::IsItemClicked())
					m_CurrentDirectory = entry.path();

				if (opened && hasSubFolders)
				{
					DrawFolderTree(entry.path(), base);
					ImGui::TreePop();
				}

				ImGui::PopID();
			}
		}
		catch (...) {}
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_Text, EditorStyle::COLOR_TITLE_TEXT);
		ImGui::Begin("Project");
		ImGui::PopStyleColor();
		ImGui::PushStyleColor(ImGuiCol_Text, EditorStyle::COLOR_PANEL_TEXT);

		const std::filesystem::path projectPath = Project::GetProjectDirectory();
		if (projectPath.empty())
		{
			ImGui::Text("No Project Open.");
			ImGui::PopStyleColor();
			ImGui::End();
			ImGui::PopStyleVar(2);
			return;
		}

		// 프로젝트가 변경되었거나 디렉토리가 유효하지 않으면 루트로 이동
		if (projectPath != m_BaseDirectory || m_CurrentDirectory.empty() || !std::filesystem::exists(m_CurrentDirectory))
		{
			m_BaseDirectory = projectPath;
			const std::filesystem::path assetsDir = projectPath / "Assets";
			m_CurrentDirectory = std::filesystem::exists(assetsDir) ? assetsDir : projectPath;
		}

		// ── 헤더 바 ──────────────────────────────────────────────────────────
		const float barHeight = ImGui::GetFrameHeight() + 8.0f;
		ImGui::PushStyleColor(ImGuiCol_ChildBg, EditorStyle::COLOR_PANEL_HEADER_BG);
		ImGui::BeginChild("ProjectHeader", ImVec2(0, barHeight), false,
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		ImGui::SetCursorPos(ImVec2(8.0f, 4.0f));

		ImGui::PushStyleColor(ImGuiCol_Button,        EditorStyle::COLOR_GIZMO_BTN);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EditorStyle::COLOR_GIZMO_BTN_HOVERED);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,  EditorStyle::COLOR_GIZMO_BTN_ACTIVE);
		ImGui::PushStyleColor(ImGuiCol_Text,          EditorStyle::COLOR_GIZMO_BTN_TEXT);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);

		if (ImGui::Button("+", ImVec2(120.0f, 0)))
			ImGui::OpenPopup("ProjectCreateMenu");

		ImGui::PopStyleVar();
		ImGui::PopStyleColor(4);

		ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y));
		ImGui::PopStyleVar(2);

		if (ImGui::BeginPopup("ProjectCreateMenu"))
		{
			if (ImGui::MenuItem("Folder"))  CreateAsset("Folder");
			if (ImGui::MenuItem("Script"))  CreateAsset("Script");
			if (ImGui::MenuItem("Scene"))   CreateAsset("Scene");
			ImGui::EndPopup();
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		ImGui::EndChild();
		ImGui::PopStyleColor();
		ImGui::Separator();

		// ── Two Column Layout ─────────────────────────────────────────────────
		const float totalWidth = ImGui::GetContentRegionAvail().x;
		const float treeWidth  = totalWidth * FOLDER_TREE_RATIO;

		// 왼쪽: 폴더 트리 (Root 노드 없이 바로 하위 폴더부터 표시)
		ImGui::PushStyleColor(ImGuiCol_ChildBg, EditorStyle::COLOR_WINDOW_BG);
		ImGui::BeginChild("ProjectFolderTree", ImVec2(treeWidth, 0), false);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 4, 2 });

		DrawFolderTree(m_BaseDirectory, m_BaseDirectory);

		ImGui::PopStyleVar();
		ImGui::EndChild();
		ImGui::PopStyleColor();

		ImGui::SameLine(0, 0);

		// 세로 구분선
		ImGui::PushStyleColor(ImGuiCol_Separator, EditorStyle::COLOR_SEPARATOR);
		{
			const float lineX = ImGui::GetCursorScreenPos().x;
			const float lineY1 = ImGui::GetCursorScreenPos().y;
			const float lineY2 = lineY1 + ImGui::GetContentRegionAvail().y;
			ImGui::GetWindowDrawList()->AddLine(ImVec2(lineX, lineY1), ImVec2(lineX, lineY2),
				ImGui::GetColorU32(EditorStyle::COLOR_SEPARATOR), 1.0f);
		}
		ImGui::PopStyleColor();

		// 오른쪽: 파일 리스트
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ LIST_PADDING, LIST_PADDING });
		ImGui::BeginChild("ProjectFileList", ImVec2(0, 0), false);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 4, 2 });

		try
		{
			// 폴더 먼저, 파일 나중
			for (int pass = 0; pass < 2; ++pass)
			{
				for (const auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
				{
					const bool isDir = directoryEntry.is_directory();
					if (pass == 0 && !isDir) continue;
					if (pass == 1 &&  isDir) continue;

					const auto& path           = directoryEntry.path();
					const std::string filename  = path.filename().string();
					const std::string label     = std::string("[") + GetFileIcon(directoryEntry) + "]  " + filename;
					const bool isRenaming       = (m_RenamingPath == path);

					ImGui::PushID(filename.c_str());

					if (isRenaming)
					{
						ImGui::SetNextItemWidth(-1);
						if (ImGui::InputText("##rename", m_RenameBuffer, RENAME_BUFFER_SIZE,
							ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
						{
							if (m_RenameBuffer[0] != '\0')
							{
								std::filesystem::path newPath = path.parent_path() / m_RenameBuffer;
								if (!std::filesystem::exists(newPath))
									std::filesystem::rename(path, newPath);
							}
							m_RenamingPath.clear();
						}
						if (ImGui::IsItemDeactivated() && !ImGui::IsKeyPressed(ImGuiKey_Enter))
							m_RenamingPath.clear();
					}
					else
					{
						const bool selected = false;
						if (ImGui::Selectable(label.c_str(), selected, ImGuiSelectableFlags_AllowDoubleClick))
						{
							if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
							{
								if (isDir)
									m_CurrentDirectory /= path.filename();
								else if (path.extension() == ".proj")
									Application::Get().OpenProject(path);
								else if (path.extension() == ".proto")
									Application::Get().OpenScene(path);
							}
						}

						// 아이템 우클릭 메뉴
						if (ImGui::BeginPopupContextItem("##ItemContext"))
						{
							if (ImGui::MenuItem("Rename"))
							{
								m_RenamingPath = path;
								strncpy_s(m_RenameBuffer, RENAME_BUFFER_SIZE, filename.c_str(), RENAME_BUFFER_SIZE - 1);
								ImGui::CloseCurrentPopup();
							}
							if (ImGui::MenuItem("Delete"))
							{
								if (isDir)
									std::filesystem::remove_all(path);
								else
									std::filesystem::remove(path);
								ImGui::CloseCurrentPopup();
							}
							ImGui::EndPopup();
						}
					}

					ImGui::PopID();
				}
			}
		}
		catch (...)
		{
			ImGui::Text("Error reading directory.");
		}

		// 빈 공간 우클릭 메뉴 (+버튼과 동일, 커서 위치에 표시)
		if (ImGui::BeginPopupContextWindow("##EmptyContext", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Folder"))  CreateAsset("Folder");
			if (ImGui::MenuItem("Script"))  CreateAsset("Script");
			if (ImGui::MenuItem("Scene"))   CreateAsset("Scene");
			ImGui::EndPopup();
		}

		ImGui::PopStyleVar();
		ImGui::EndChild();
		ImGui::PopStyleVar();

		ImGui::PopStyleColor();
		ImGui::End();
		ImGui::PopStyleVar(2);
	}
}

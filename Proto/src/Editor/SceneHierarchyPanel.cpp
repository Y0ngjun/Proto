/*
 * 씬 내의 게임 오브젝트 계층 구조를 보여주는 에디터 패널 클래스입니다.
 * 오브젝트 선택, 생성, 삭제 및 이름 변경 기능을 제공합니다.
 */

#include <imgui.h>

#include "SceneHierarchyPanel.h"
#include "../Scene/Components/Transform.h"
#include "../Scene/Components/MeshRenderer.h"
#include "../Asset/AssetManager.h"
#include "../Asset/Asset.h"
#include "../Renderer/VertexArray.h"
#include "../Renderer/Shader.h"
#include "../Scene/GameObject.h"
#include "../Scene/Scene.h"

namespace Proto
{
	SceneHierarchyPanel::SceneHierarchyPanel(Scene* context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(Scene* context)
	{
		if (m_Context != context)
		{
			m_Context = context;
			m_SelectionContext = nullptr;
		}
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
		ImGui::Begin("Hierarchy");

		// 헤더 바: 통일된 스타일 (COLOR_VIEWPORT_HEADER 배경)
		const float barHeight = ImGui::GetFrameHeight() + 8.0f;
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
		ImGui::BeginChild("HierarchyHeader", ImVec2(0, barHeight), false,
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		ImGui::SetCursorPos(ImVec2(8.0f, 4.0f));

		const bool canCreate = (m_Context != nullptr);
		if (!canCreate)
		{
			ImGui::BeginDisabled();
		}

		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
		if (ImGui::Button("+ Add", ImVec2(120.0f, 0)))
		{
			ImGui::OpenPopup("HierarchyCreateMenu");
		}
		ImGui::PopStyleVar();

		if (!canCreate)
		{
			ImGui::EndDisabled();
		}

		if (ImGui::BeginPopup("HierarchyCreateMenu"))
		{
			if (ImGui::MenuItem("Empty Object"))
			{
				m_Context->CreateGameObject("Empty Object");
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Cube"))
			{
				m_Context->CreateMeshGameObject("Cube", UUID(DefaultAsset::CUBE));
			}

			if (ImGui::MenuItem("Sphere"))
			{
				m_Context->CreateMeshGameObject("Sphere", UUID(DefaultAsset::SPHERE));
			}

			if (ImGui::MenuItem("Plane"))
			{
				m_Context->CreateMeshGameObject("Plane", UUID(DefaultAsset::PLANE));
			}

			if (ImGui::MenuItem("Cylinder"))
			{
				m_Context->CreateMeshGameObject("Cylinder", UUID(DefaultAsset::CYLINDER));
			}

			ImGui::EndPopup();
		}

		ImGui::EndChild();
		ImGui::PopStyleColor();
		ImGui::Separator();

		if (!m_Context)
		{
			ImGui::End();
			ImGui::PopStyleVar(2);
			return;
		}

		for (auto& go : m_Context->GetGameObjects())
		{
			DrawEntityNode(go.get());
		}

		// Delete 키로 오브젝트 삭제 (포커스된 상태에서만)
		if (m_SelectionContext && !m_RenamingGameObject && ImGui::IsWindowFocused() && ImGui::IsKeyPressed(ImGuiKey_Delete))
		{
			m_Context->RemoveGameObject(m_SelectionContext);
			m_SelectionContext = nullptr;
		}

		// 배경 클릭 시 선택 해제 (아이템이나 상단바를 클릭하지 않았을 때만)
		if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered())
		{
			m_SelectionContext = nullptr;
			m_RenamingGameObject = nullptr;
		}

		if (ImGui::BeginPopupContextWindow("##CreateObject", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Create Empty Object"))
			{
				m_Context->CreateGameObject("Empty Object");
			}

			if (ImGui::MenuItem("Create Cube"))
			{
				m_Context->CreateMeshGameObject("Cube", UUID(DefaultAsset::CUBE));
			}

			if (ImGui::MenuItem("Create Sphere"))
			{
				m_Context->CreateMeshGameObject("Sphere", UUID(DefaultAsset::SPHERE));
			}

			if (ImGui::MenuItem("Create Cylinder"))
			{
				m_Context->CreateMeshGameObject("Cylinder", UUID(DefaultAsset::CYLINDER));
			}

			if (ImGui::MenuItem("Create Plane"))
			{
				m_Context->CreateMeshGameObject("Plane", UUID(DefaultAsset::PLANE));
			}

			ImGui::EndPopup();
		}

		if (m_RenamingGameObject)
		{
			ImGui::SetNextItemWidth(-1);
			if (ImGui::InputText("##RenameInput", m_RenamingBuffer, RENAME_BUFFER_SIZE, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				if (m_RenamingBuffer[0] != '\0')
				{
					m_RenamingGameObject->SetName(m_RenamingBuffer);
				}

				m_RenamingGameObject = nullptr;
			}

			if (ImGui::IsItemDeactivated() && !ImGui::IsKeyPressed(ImGuiKey_Enter))
			{
				m_RenamingGameObject = nullptr;
			}
		}

		ImGui::End();
		ImGui::PopStyleVar(2);
	}

	void SceneHierarchyPanel::DrawEntityNode(GameObject* gameObject)
	{
		ImGui::PushID((void*)gameObject);

		ImGuiTreeNodeFlags flags = ((m_SelectionContext == gameObject) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		const bool opened = ImGui::TreeNodeEx((void*)gameObject, flags, "%s", gameObject->GetName().c_str());

		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = gameObject;
		}

		if (ImGui::BeginPopupContextItem("EntityContextMenu"))
		{
			if (ImGui::MenuItem("Rename"))
			{
				m_RenamingGameObject = gameObject;
				strncpy_s(m_RenamingBuffer, RENAME_BUFFER_SIZE, gameObject->GetName().c_str(), RENAME_BUFFER_SIZE - 1);
				m_RenamingBuffer[RENAME_BUFFER_SIZE - 1] = '\0';
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Delete"))
			{
				if (m_SelectionContext == gameObject)
				{
					m_SelectionContext = nullptr;
				}

				if (m_RenamingGameObject == gameObject)
				{
					m_RenamingGameObject = nullptr;
				}

				m_Context->RemoveGameObject(gameObject);
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		if (opened)
		{
			ImGui::TreePop();
		}

		ImGui::PopID();
	}
}

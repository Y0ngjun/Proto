#include <imgui.h>

#include "SceneHierarchyPanel.h"

#include "../Scene/Components/Transform.h"
#include "../Scene/Components/MeshRenderer.h"
#include "../Asset/AssetManager.h"
#include "../Renderer/VertexArray.h"
#include "../Renderer/Shader.h"

namespace Proto
{
	SceneHierarchyPanel::SceneHierarchyPanel(Scene* context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(Scene* context)
	{
		m_Context = context;
		m_SelectionContext = nullptr;
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Hierarchy");
		if (m_Context)
		{
			for (auto& go : m_Context->GetGameObjects())
			{
				DrawEntityNode(go.get());
			}

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
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
					m_Context->CreateMeshGameObject("Cube", UUID(1));
				}
				if (ImGui::MenuItem("Create Sphere"))
				{
					m_Context->CreateMeshGameObject("Sphere", UUID(3));
				}
				if (ImGui::MenuItem("Create Cylinder"))
				{
					m_Context->CreateMeshGameObject("Cylinder", UUID(4));
				}
				if (ImGui::MenuItem("Create Plane"))
				{
					m_Context->CreateMeshGameObject("Plane", UUID(2));
				}
				ImGui::EndPopup();
			}

			// 이름 편집 처리
			if (m_RenamingGameObject)
			{
				ImGui::SetNextItemWidth(-1);
				ImGui::InputText("##RenameInput", m_RenamingBuffer, RenameBufferSize, ImGuiInputTextFlags_EnterReturnsTrue);

				if (ImGui::IsItemDeactivated())
				{
					if (ImGui::IsKeyPressed(ImGuiKey_Enter) && m_RenamingBuffer[0] != '\0')
					{
						m_RenamingGameObject->SetName(m_RenamingBuffer);
					}
					m_RenamingGameObject = nullptr;
					m_RenamingBuffer[0] = '\0';
				}
			}
		}
		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(GameObject* gameObject)
	{
		ImGuiTreeNodeFlags flags = ((m_SelectionContext == gameObject) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		bool opened = ImGui::TreeNodeEx((void*)gameObject, flags, "%s", gameObject->GetName().c_str());

		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = gameObject;
		}

		// 우클릭 메뉴
		if (ImGui::BeginPopupContextItem("EntityContextMenu"))
		{
			if (ImGui::MenuItem("Rename"))
			{
				m_RenamingGameObject = gameObject;
				strncpy_s(m_RenamingBuffer, RenameBufferSize, gameObject->GetName().c_str(), RenameBufferSize - 1);
				m_RenamingBuffer[RenameBufferSize - 1] = '\0';
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
	}

}

#include <imgui.h>

#include "SceneHierarchyPanel.h"

#include "../Scene/Components/Transform.h"

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
		ImGui::Begin("Scene Hierarchy");
		if (m_Context)
		{
			for (auto& go : m_Context->GetGameObjects())
			{
				DrawEntityNode(go.get());
			}

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			{
				m_SelectionContext = nullptr;
			}
		}
		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(GameObject* gameObject)
	{
		ImGuiTreeNodeFlags flags = ((m_SelectionContext == gameObject) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)gameObject, flags, "%s", gameObject->GetName().c_str());

		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = gameObject;
		}

		if (opened)
		{
			ImGui::TreePop();
		}
	}

}
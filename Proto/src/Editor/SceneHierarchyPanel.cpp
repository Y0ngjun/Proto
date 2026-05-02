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

			if (ImGui::BeginPopupContextWindow("##CreateObject", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
			{
				if (ImGui::MenuItem("Create Empty Object"))
				{
					m_Context->CreateGameObject("Empty Object");
				}
				if (ImGui::MenuItem("Create Cube"))
				{
					auto* go = m_Context->CreateGameObject("Cube");
					auto* mr = go->AddComponent<MeshRenderer>();
					mr->SetMesh(AssetManager::GetAssetAs<VertexArray>(UUID(1)));
					mr->SetShader(AssetManager::GetAssetAs<Shader>(UUID(100)));
				}
				if (ImGui::MenuItem("Create Sphere"))
				{
					auto* go = m_Context->CreateGameObject("Sphere");
					auto* mr = go->AddComponent<MeshRenderer>();
					mr->SetMesh(AssetManager::GetAssetAs<VertexArray>(UUID(3)));
					mr->SetShader(AssetManager::GetAssetAs<Shader>(UUID(100)));
				}
				if (ImGui::MenuItem("Create Cylinder"))
				{
					auto* go = m_Context->CreateGameObject("Cylinder");
					auto* mr = go->AddComponent<MeshRenderer>();
					mr->SetMesh(AssetManager::GetAssetAs<VertexArray>(UUID(4)));
					mr->SetShader(AssetManager::GetAssetAs<Shader>(UUID(100)));
				}
				if (ImGui::MenuItem("Create Plane"))
				{
					auto* go = m_Context->CreateGameObject("Plane");
					auto* mr = go->AddComponent<MeshRenderer>();
					mr->SetMesh(AssetManager::GetAssetAs<VertexArray>(UUID(2)));
					mr->SetShader(AssetManager::GetAssetAs<Shader>(UUID(100)));
				}
				ImGui::EndPopup();
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
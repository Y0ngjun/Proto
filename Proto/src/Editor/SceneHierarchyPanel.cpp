#include "SceneHierarchyPanel.h"
#include <imgui.h>
#include "../Scene/Components/Transform.h"

namespace Proto {

	SceneHierarchyPanel::SceneHierarchyPanel(Scene* context) {
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(Scene* context) {
		m_Context = context;
		m_SelectionContext = nullptr;
	}

	void SceneHierarchyPanel::OnImGuiRender() {
		ImGui::Begin("Scene Hierarchy");

		if (m_Context) {
			for (auto& go : m_Context->GetGameObjects()) {
				DrawEntityNode(go.get());
			}

			// 우클릭으로 빈 공간 클릭시 선택 해제
			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
				m_SelectionContext = nullptr;
		}

		ImGui::End();

		ImGui::Begin("Inspector");
		if (m_SelectionContext) {
			DrawComponents(m_SelectionContext);
		}
		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(GameObject* gameObject) {
		ImGuiTreeNodeFlags flags = ((m_SelectionContext == gameObject) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)gameObject, flags, "%s", gameObject->GetName().c_str());
		
		if (ImGui::IsItemClicked()) {
			m_SelectionContext = gameObject;
		}

		if (opened) {
			ImGui::TreePop();
		}
	}

	void SceneHierarchyPanel::DrawComponents(GameObject* gameObject) {
		if (gameObject->GetName() != "") {
			ImGui::Text("Name: %s", gameObject->GetName().c_str());
			ImGui::Separator();
		}

		if (auto transform = gameObject->GetComponent<Transform>()) {
			if (ImGui::TreeNodeEx((void*)typeid(Transform).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform")) {
				
				ImGui::DragFloat3("Translation", &transform->Translation.x, 0.1f);
				
				glm::vec3 rotationDegrees = glm::degrees(transform->Rotation);
				if (ImGui::DragFloat3("Rotation", &rotationDegrees.x, 0.1f)) {
					transform->Rotation = glm::radians(rotationDegrees);
				}
				
				ImGui::DragFloat3("Scale", &transform->Scale.x, 0.1f);

				ImGui::TreePop();
			}
		}

		// TODO: 차후에 MeshRenderer 컴포넌트 정보 등도 이곳에 추가
	}

}
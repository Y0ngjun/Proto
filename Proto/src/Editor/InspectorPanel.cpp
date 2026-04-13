#include <imgui.h>

#include "InspectorPanel.h"

#include "../Scene/Components/Transform.h"

namespace Proto
{
	void InspectorPanel::OnImGuiRender(GameObject* selectedContext)
	{
		ImGui::Begin("Inspector");
		if (selectedContext)
		{
			DrawComponents(selectedContext);
		}
		ImGui::End();
	}

	void InspectorPanel::DrawComponents(GameObject* gameObject)
	{
		if (gameObject->GetName() != "")
		{
			ImGui::Text("Name: %s", gameObject->GetName().c_str());
			ImGui::Separator();
		}

		if (auto transform = gameObject->GetComponent<Transform>())
		{
			if (ImGui::TreeNodeEx((void*)typeid(Transform).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
			{
				ImGui::DragFloat3("Translation", &transform->Translation.x, 0.1f);

				glm::vec3 rotationDegrees = glm::degrees(transform->Rotation);
				if (ImGui::DragFloat3("Rotation", &rotationDegrees.x, 0.1f))
				{
					transform->Rotation = glm::radians(rotationDegrees);
				}

				ImGui::DragFloat3("Scale", &transform->Scale.x, 0.1f);

				ImGui::TreePop();
			}
		}

		// TODO: 차후에 MeshRenderer 컴포넌트 정보 등도 이곳에 추가
	}
}

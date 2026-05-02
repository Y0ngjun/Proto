#include <imgui.h>
#include "../../Dependencies/ImGuizmo/ImGuizmo.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "InspectorPanel.h"
#include "../Core/Application.h"
#include "../Scene/Components/Transform.h"
#include "../Scene/Components/MeshRenderer.h"
#include "../Scene/Components/CameraComponent.h"
#include "../Scene/Components/LightComponent.h"

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

		if (auto meshRenderer = gameObject->GetComponent<MeshRenderer>())
		{
			if (ImGui::TreeNodeEx((void*)typeid(MeshRenderer).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "MeshRenderer"))
			{
				const std::string& meshTypeName = meshRenderer->GetMeshTypeName();
				ImGui::Text("Mesh Type: %s", meshTypeName.empty() ? "(None)" : meshTypeName.c_str());

				if (meshRenderer->GetMesh())
				{
					ImGui::Text("Mesh: Loaded");
				}
				else
				{
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Mesh: Not Set");
				}

				if (meshRenderer->GetShader())
				{
					ImGui::Text("Shader: Loaded");
				}
				else
				{
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Shader: Not Set");
				}

				ImGui::TreePop();
			}
		}

		if (auto camera = gameObject->GetComponent<CameraComponent>())
		{
			if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "CameraComponent"))
			{
				bool primaryChanged = false;
				bool primary = camera->Primary;
				if (ImGui::Checkbox("Primary", &primary))
				{
					camera->Primary = primary;
					primaryChanged = true;
				}

				bool fixedAspect = camera->FixedAspectRatio;
				ImGui::Checkbox("Fixed Aspect Ratio", &fixedAspect);
				if (fixedAspect != camera->FixedAspectRatio)
				{
					camera->FixedAspectRatio = fixedAspect;
				}

				float fov = glm::degrees(camera->Camera.GetPerspectiveVerticalFOV());
				if (ImGui::SliderFloat("FOV", &fov, 1.0f, 179.0f))
				{
					camera->Camera.SetPerspective(glm::radians(fov), 0.1f, 1000.0f);
				}

				ImGui::TreePop();
			}
		}

		if (auto light = gameObject->GetComponent<LightComponent>())
		{
			if (ImGui::TreeNodeEx((void*)typeid(LightComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "LightComponent"))
			{
				ImGui::ColorEdit3("Light Color", &light->Color.x);
				ImGui::SliderFloat("Intensity", &light->Intensity, 0.0f, 2.0f);
				ImGui::TreePop();
			}
		}
	}
}

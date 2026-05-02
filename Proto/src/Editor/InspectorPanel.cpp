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

		DrawGizmoControls();
		ImGui::Separator();

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

	void InspectorPanel::DrawGizmoControls()
	{
		ImGui::Text("Gizmo Mode");
		ImGui::SameLine();

		Application& app = Application::Get();
		int currentGizmoType = app.GetGizmoType();

		ImGui::PushID("GizmoButtons");

		float buttonWidth = 80.0f;
		float spacing = 8.0f;

		// 선택되지 않은 버튼의 기본 스타일
		ImVec4 buttonColor(0.3f, 0.3f, 0.3f, 1.0f);
		ImVec4 buttonHoveredColor(0.4f, 0.4f, 0.4f, 1.0f);
		ImVec4 buttonActiveColor(0.25f, 0.25f, 0.25f, 1.0f);

		// 선택된 버튼의 스타일 (밝은 회색)
		ImVec4 selectedButtonColor(0.5f, 0.5f, 0.5f, 1.0f);
		ImVec4 selectedButtonHoveredColor(0.6f, 0.6f, 0.6f, 1.0f);
		ImVec4 selectedButtonActiveColor(0.45f, 0.45f, 0.45f, 1.0f);

		// None (비활성화)
		if (currentGizmoType == -1)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, selectedButtonColor);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, selectedButtonHoveredColor);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, selectedButtonActiveColor);
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonHoveredColor);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonActiveColor);
		}
		if (ImGui::Button("None", ImVec2(buttonWidth, 0)))
		{
			app.SetGizmoType(-1);
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine(0, spacing);

		// Move (이동)
		if (currentGizmoType == ImGuizmo::OPERATION::TRANSLATE)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, selectedButtonColor);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, selectedButtonHoveredColor);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, selectedButtonActiveColor);
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonHoveredColor);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonActiveColor);
		}
		if (ImGui::Button("Move", ImVec2(buttonWidth, 0)))
		{
			app.SetGizmoType(ImGuizmo::OPERATION::TRANSLATE);
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine(0, spacing);

		// Rotate (회전)
		if (currentGizmoType == ImGuizmo::OPERATION::ROTATE)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, selectedButtonColor);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, selectedButtonHoveredColor);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, selectedButtonActiveColor);
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonHoveredColor);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonActiveColor);
		}
		if (ImGui::Button("Rotate", ImVec2(buttonWidth, 0)))
		{
			app.SetGizmoType(ImGuizmo::OPERATION::ROTATE);
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine(0, spacing);

		// Scale (스케일)
		if (currentGizmoType == ImGuizmo::OPERATION::SCALE)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, selectedButtonColor);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, selectedButtonHoveredColor);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, selectedButtonActiveColor);
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonHoveredColor);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonActiveColor);
		}
		if (ImGui::Button("Scale", ImVec2(buttonWidth, 0)))
		{
			app.SetGizmoType(ImGuizmo::OPERATION::SCALE);
		}
		ImGui::PopStyleColor(3);

		ImGui::PopID();

		ImGui::SameLine();
		ImGui::TextDisabled("(Q/W/E/R)");
	}
}

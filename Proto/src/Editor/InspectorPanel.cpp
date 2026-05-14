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
#include "../Scene/Components/Rigidbody.h"
#include "../Scene/Components/BoxCollider.h"
#include "../Scene/Components/SphereCollider.h"
#include "../Scene/Components/NativeScriptComponent.h"
#include "../Scene/ScriptRegistry.h"

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

		// Add Component 버튼
		DrawAddComponentButton(gameObject);
		ImGui::Spacing();

		// Transform (기본 컴포넌트, 삭제 불가)
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

		// MeshRenderer (삭제 가능)
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

				// 삭제 버튼
				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.0f, 0.0f, 0.5f));
				if (ImGui::Button("X##MeshRenderer", ImVec2(25, 0)))
				{
					gameObject->RemoveComponent<MeshRenderer>();
				}
				ImGui::PopStyleColor();
			}
		}

		// CameraComponent (삭제 가능)
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

				// 삭제 버튼
				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.0f, 0.0f, 0.5f));
				if (ImGui::Button("X##CameraComponent", ImVec2(25, 0)))
				{
					gameObject->RemoveComponent<CameraComponent>();
				}
				ImGui::PopStyleColor();
			}
		}

		// LightComponent (삭제 가능)
		if (auto light = gameObject->GetComponent<LightComponent>())
		{
			if (ImGui::TreeNodeEx((void*)typeid(LightComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "LightComponent"))
			{
				ImGui::ColorEdit3("Light Color", &light->Color.x);
				ImGui::SliderFloat("Intensity", &light->Intensity, 0.0f, 2.0f);
				ImGui::TreePop();

				// 삭제 버튼
				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.0f, 0.0f, 0.5f));
				if (ImGui::Button("X##LightComponent", ImVec2(25, 0)))
				{
					gameObject->RemoveComponent<LightComponent>();
				}
				ImGui::PopStyleColor();
			}
		}

		// Rigidbody (삭제 가능)
		if (auto rigidbody = gameObject->GetComponent<Rigidbody>())
		{
			if (ImGui::TreeNodeEx((void*)typeid(Rigidbody).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Rigidbody"))
			{
				ImGui::SliderFloat("Mass", &rigidbody->Mass, 0.1f, 100.0f);
				ImGui::DragFloat3("Velocity", &rigidbody->Velocity.x, 0.1f);
				ImGui::Checkbox("Use Gravity", &rigidbody->UseGravity);
				ImGui::SliderFloat("Drag", &rigidbody->Drag, 0.0f, 10.0f);
				ImGui::TreePop();

				// 삭제 버튼
				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.0f, 0.0f, 0.5f));
				if (ImGui::Button("X##Rigidbody", ImVec2(25, 0)))
				{
					gameObject->RemoveComponent<Rigidbody>();
				}
				ImGui::PopStyleColor();
			}
		}

		// BoxCollider (삭제 가능)
		if (auto boxCollider = gameObject->GetComponent<BoxCollider>())
		{
			if (ImGui::TreeNodeEx((void*)typeid(BoxCollider).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "BoxCollider"))
			{
				ImGui::DragFloat3("Size", &boxCollider->Size.x, 0.1f, 0.1f, 100.0f);
				ImGui::TreePop();

				// 삭제 버튼
				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.0f, 0.0f, 0.5f));
				if (ImGui::Button("X##BoxCollider", ImVec2(25, 0)))
				{
					gameObject->RemoveComponent<BoxCollider>();
				}
				ImGui::PopStyleColor();
			}
		}

		// SphereCollider (삭제 가능)
		if (auto sphereCollider = gameObject->GetComponent<SphereCollider>())
		{
			if (ImGui::TreeNodeEx((void*)typeid(SphereCollider).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "SphereCollider"))
			{
				ImGui::SliderFloat("Radius", &sphereCollider->Radius, 0.1f, 50.0f);
				ImGui::TreePop();

				// 삭제 버튼
				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.0f, 0.0f, 0.5f));
				if (ImGui::Button("X##SphereCollider", ImVec2(25, 0)))
				{
					gameObject->RemoveComponent<SphereCollider>();
				}
				ImGui::PopStyleColor();
			}
		}

		// NativeScriptComponent (삭제 가능)
		if (auto nativeScript = gameObject->GetComponent<NativeScriptComponent>())
		{
			if (ImGui::TreeNodeEx((void*)typeid(NativeScriptComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "NativeScriptComponent"))
			{
				// 등록된 모든 스크립트 목록 가져오기
				std::vector<std::string> scriptNames = ScriptRegistry::GetScriptNames();

				// 현재 할당된 스크립트 찾기
				int currentScriptIndex = -1;
				for (int i = 0; i < (int)scriptNames.size(); ++i)
				{
					if (scriptNames[i] == nativeScript->ScriptName)
					{
						currentScriptIndex = i;
						break;
					}
				}

				// 드롭다운 목록을 위한 char* 배열 생성
				std::vector<const char*> scriptNamesCStr;
				for (const auto& name : scriptNames)
				{
					scriptNamesCStr.push_back(name.c_str());
				}

				// 드롭다운 생성
				if (!scriptNamesCStr.empty())
				{
					const char* previewValue = (currentScriptIndex >= 0) ? scriptNamesCStr[currentScriptIndex] : "(None)";
					if (ImGui::Combo("##Script", &currentScriptIndex, scriptNamesCStr.data(), (int)scriptNamesCStr.size()))
					{
						if (currentScriptIndex >= 0 && currentScriptIndex < (int)scriptNames.size())
						{
							ScriptRegistry::BindByName(scriptNames[currentScriptIndex], nativeScript);
						}
					}
				}
				else
				{
					ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "No scripts registered");
				}

				// 현재 할당된 스크립트 표시
				if (!nativeScript->ScriptName.empty())
				{
					ImGui::Text("Current Script: %s", nativeScript->ScriptName.c_str());
					if (nativeScript->Instance)
					{
						ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Instance: Created");
					}
					else
					{
						ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Instance: Not created (needs OnStart)");
					}
				}

				ImGui::TreePop();

				// 삭제 버튼
				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.0f, 0.0f, 0.5f));
				if (ImGui::Button("X##NativeScriptComponent", ImVec2(25, 0)))
				{
					gameObject->RemoveComponent<NativeScriptComponent>();
				}
				ImGui::PopStyleColor();
			}
		}
	}

	void InspectorPanel::DrawAddComponentButton(GameObject* gameObject)
	{
		if (ImGui::Button("Add Component", ImVec2(-1, 0)))
		{
			ImGui::OpenPopup("AddComponentPopup");
		}

		if (ImGui::BeginPopup("AddComponentPopup"))
		{
			// MeshRenderer
			if (!gameObject->GetComponent<MeshRenderer>())
			{
				if (ImGui::MenuItem("MeshRenderer"))
				{
					gameObject->AddComponent<MeshRenderer>();
					ImGui::CloseCurrentPopup();
				}
			}
			else
			{
				ImGui::TextDisabled("MeshRenderer");
			}

			// CameraComponent
			if (!gameObject->GetComponent<CameraComponent>())
			{
				if (ImGui::MenuItem("CameraComponent"))
				{
					gameObject->AddComponent<CameraComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			else
			{
				ImGui::TextDisabled("CameraComponent");
			}

			// LightComponent
			if (!gameObject->GetComponent<LightComponent>())
			{
				if (ImGui::MenuItem("LightComponent"))
				{
					gameObject->AddComponent<LightComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			else
			{
				ImGui::TextDisabled("LightComponent");
			}

			ImGui::Separator();

			// Rigidbody
			if (!gameObject->GetComponent<Rigidbody>())
			{
				if (ImGui::MenuItem("Rigidbody"))
				{
					gameObject->AddComponent<Rigidbody>();
					ImGui::CloseCurrentPopup();
				}
			}
			else
			{
				ImGui::TextDisabled("Rigidbody");
			}

			// BoxCollider
			if (!gameObject->GetComponent<BoxCollider>())
			{
				if (ImGui::MenuItem("BoxCollider"))
				{
					gameObject->AddComponent<BoxCollider>();
					ImGui::CloseCurrentPopup();
				}
			}
			else
			{
				ImGui::TextDisabled("BoxCollider");
			}

			// SphereCollider
			if (!gameObject->GetComponent<SphereCollider>())
			{
				if (ImGui::MenuItem("SphereCollider"))
				{
					gameObject->AddComponent<SphereCollider>();
					ImGui::CloseCurrentPopup();
				}
			}
			else
			{
				ImGui::TextDisabled("SphereCollider");
			}

			// NativeScriptComponent
			if (!gameObject->GetComponent<NativeScriptComponent>())
			{
				if (ImGui::MenuItem("NativeScriptComponent"))
				{
					gameObject->AddComponent<NativeScriptComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			else
			{
				ImGui::TextDisabled("NativeScriptComponent");
			}

			ImGui::EndPopup();
		}
	}
}


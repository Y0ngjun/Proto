#include <imgui.h>
#include <imgui_internal.h>
#include "../../Dependencies/ImGuizmo/ImGuizmo.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "InspectorPanel.h"
#include "../Core/Application.h"
#include "../Scene/GameObject.h"
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
	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, GameObject* gameObject, UIFunction uiFunction, bool canRemove = true)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_FramePadding;
		if (gameObject->HasComponent<T>())
		{
			auto& component = gameObject->GetComponentRef<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = GImGui->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar();

			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			if (canRemove && ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove Component"))
					removeComponent = true;

				ImGui::EndPopup();
			}

			if (open)
			{
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				gameObject->RemoveComponent<T>();
		}
	}

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

		DrawAddComponentButton(gameObject);
		ImGui::Spacing();

		DrawComponent<Transform>("Transform", gameObject, [](auto& component)
		{
			ImGui::DragFloat3("Translation", &component.Translation.x, 0.1f);
			glm::vec3 rotationDegrees = glm::degrees(component.Rotation);
			if (ImGui::DragFloat3("Rotation", &rotationDegrees.x, 0.1f))
				component.Rotation = glm::radians(rotationDegrees);
			ImGui::DragFloat3("Scale", &component.Scale.x, 0.1f);
		}, false);

		DrawComponent<MeshRenderer>("Mesh Renderer", gameObject, [](auto& component)
		{
			const std::string& meshTypeName = component.GetMeshTypeName();
			ImGui::Text("Mesh Type: %s", meshTypeName.empty() ? "(None)" : meshTypeName.c_str());
			
			if (component.GetMesh())
				ImGui::Text("Mesh: Loaded");
			else
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Mesh: Not Set");

			if (component.GetShader())
				ImGui::Text("Shader: Loaded");
			else
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Shader: Not Set");
		});

		DrawComponent<CameraComponent>("Camera", gameObject, [](auto& component)
		{
			ImGui::Checkbox("Primary", &component.Primary);
			ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);

			float fov = glm::degrees(component.Camera.GetPerspectiveVerticalFOV());
			if (ImGui::SliderFloat("FOV", &fov, 1.0f, 179.0f))
				component.Camera.SetPerspective(glm::radians(fov), 0.1f, 1000.0f);
		});

		DrawComponent<LightComponent>("Light", gameObject, [](auto& component)
		{
			ImGui::ColorEdit3("Color", &component.Color.x);
			ImGui::SliderFloat("Intensity", &component.Intensity, 0.0f, 2.0f);
		});

		DrawComponent<Rigidbody>("Rigidbody", gameObject, [](auto& component)
		{
			ImGui::SliderFloat("Mass", &component.Mass, 0.1f, 100.0f);
			ImGui::DragFloat3("Velocity", &component.Velocity.x, 0.1f);
			ImGui::Checkbox("Use Gravity", &component.UseGravity);
			ImGui::SliderFloat("Drag", &component.Drag, 0.0f, 10.0f);
		});

		DrawComponent<BoxCollider>("Box Collider", gameObject, [](auto& component)
		{
			ImGui::DragFloat3("Size", &component.Size.x, 0.1f, 0.1f, 100.0f);
		});

		DrawComponent<SphereCollider>("Sphere Collider", gameObject, [](auto& component)
		{
			ImGui::SliderFloat("Radius", &component.Radius, 0.1f, 50.0f);
		});

		DrawComponent<NativeScriptComponent>("Native Script", gameObject, [](auto& component)
		{
			std::vector<std::string> scriptNames = ScriptRegistry::GetScriptNames();
			int currentScriptIndex = -1;
			for (int i = 0; i < (int)scriptNames.size(); ++i)
			{
				if (scriptNames[i] == component.ScriptName)
				{
					currentScriptIndex = i;
					break;
				}
			}

			std::vector<const char*> scriptNamesCStr;
			for (const auto& name : scriptNames)
				scriptNamesCStr.push_back(name.c_str());

			if (!scriptNamesCStr.empty())
			{
				if (ImGui::Combo("##Script", &currentScriptIndex, scriptNamesCStr.data(), (int)scriptNamesCStr.size()))
				{
					if (currentScriptIndex >= 0 && currentScriptIndex < (int)scriptNames.size())
						ScriptRegistry::BindByName(scriptNames[currentScriptIndex], &component);
				}
			}
			else
			{
				ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "No scripts registered");
			}

			if (!component.ScriptName.empty())
			{
				ImGui::Text("Current: %s", component.ScriptName.c_str());
				if (component.Instance)
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Instance: Ready");
				else
					ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Instance: Pending OnStart");
			}
		});
	}

	void InspectorPanel::DrawAddComponentButton(GameObject* gameObject)
	{
		if (ImGui::Button("Add Component", ImVec2(-1, 0)))
			ImGui::OpenPopup("AddComponentPopup");

		if (ImGui::BeginPopup("AddComponentPopup"))
		{
			auto DisplayMenuItem = [&](const std::string& name)
			{
				if (ImGui::MenuItem(name.c_str()))
				{
					if (name == "MeshRenderer") gameObject->AddComponent<MeshRenderer>();
					else if (name == "CameraComponent") gameObject->AddComponent<CameraComponent>();
					else if (name == "LightComponent") gameObject->AddComponent<LightComponent>();
					else if (name == "Rigidbody") gameObject->AddComponent<Rigidbody>();
					else if (name == "BoxCollider") gameObject->AddComponent<BoxCollider>();
					else if (name == "SphereCollider") gameObject->AddComponent<SphereCollider>();
					else if (name == "NativeScriptComponent") gameObject->AddComponent<NativeScriptComponent>();
					
					ImGui::CloseCurrentPopup();
				}
			};

			if (!gameObject->HasComponent<MeshRenderer>()) DisplayMenuItem("MeshRenderer");
			if (!gameObject->HasComponent<CameraComponent>()) DisplayMenuItem("CameraComponent");
			if (!gameObject->HasComponent<LightComponent>()) DisplayMenuItem("LightComponent");
			
			ImGui::Separator();
			
			if (!gameObject->HasComponent<Rigidbody>()) DisplayMenuItem("Rigidbody");
			if (!gameObject->HasComponent<BoxCollider>()) DisplayMenuItem("BoxCollider");
			if (!gameObject->HasComponent<SphereCollider>()) DisplayMenuItem("SphereCollider");
			if (!gameObject->HasComponent<NativeScriptComponent>()) DisplayMenuItem("NativeScriptComponent");

			ImGui::EndPopup();
		}
	}
}

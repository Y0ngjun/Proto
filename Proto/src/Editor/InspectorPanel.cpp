/*
 * 선택된 게임 오브젝트의 컴포넌트 정보 및 상세 설정을 표시하는 인스펙터 패널 클래스입니다.
 * 컴포넌트 추가/삭제 및 각 속성(변환, 렌더러, 물리 등)의 실시간 수정을 지원합니다.
 */

#include <imgui.h>
#include <imgui_internal.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../Dependencies/ImGuizmo/ImGuizmo.h"
#include "InspectorPanel.h"
#include "EditorStyle.h"
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
#include "../Scene/Scene.h"
#include "../Asset/AssetManager.h"
#include "../Asset/Asset.h"
#include "../Renderer/Shader.h"
#include "../Renderer/VertexArray.h"
#include "../Renderer/MeshLoader.h"

namespace Proto
{
	namespace
	{
		static constexpr ImVec4 COLOR_ERROR = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
		static constexpr ImVec4 COLOR_WARNING = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);
		static constexpr ImVec4 COLOR_SUCCESS = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);

		static constexpr float DRAG_SPEED = 0.1f;
		static constexpr float DEFAULT_FOV_MIN = 1.0f;
		static constexpr float DEFAULT_FOV_MAX = 179.0f;
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, GameObject* gameObject, UIFunction uiFunction, bool canRemove = true)
	{
		if (!gameObject->HasComponent<T>())
		{
			return;
		}

		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
			ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_FramePadding;

		auto& component = gameObject->GetComponentRef<T>();
		const ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
		const float lineHeight = GImGui->FontSize + GImGui->Style.FramePadding.y * 2.0f;

		ImGui::Separator();
		const bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
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
			{
				removeComponent = true;
			}

			ImGui::EndPopup();
		}

		if (open)
		{
			uiFunction(component);
			ImGui::TreePop();
		}

		if (removeComponent)
		{
			gameObject->RemoveComponent<T>();
			Application::Get().GetActiveScene()->SetDirty(true);
		}
	}

	void InspectorPanel::OnImGuiRender(GameObject* selectedContext)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, EditorStyle::COLOR_TITLE_TEXT);
		ImGui::Begin("Inspector");
		ImGui::PopStyleColor();

		if (selectedContext)
		{
			DrawComponents(selectedContext);
		}

		ImGui::End();
	}

	void InspectorPanel::DrawComponents(GameObject* gameObject)
	{
		if (!gameObject)
		{
			return;
		}

		if (!gameObject->GetName().empty())
		{
			ImGui::Text("Name: %s", gameObject->GetName().c_str());
			ImGui::Separator();
		}

		DrawAddComponentButton(gameObject);
		ImGui::Spacing();

		// Transform 컴포넌트 드로잉
		DrawComponent<Transform>("Transform", gameObject, [](auto& component)
			{
				if (ImGui::DragFloat3("Translation", &component.Translation.x, DRAG_SPEED))
					Application::Get().GetActiveScene()->SetDirty(true);

				glm::vec3 rotationDegrees = glm::degrees(component.Rotation);
				if (ImGui::DragFloat3("Rotation", &rotationDegrees.x, DRAG_SPEED))
				{
					component.Rotation = glm::radians(rotationDegrees);
					Application::Get().GetActiveScene()->SetDirty(true);
				}

				if (ImGui::DragFloat3("Scale", &component.Scale.x, DRAG_SPEED))
					Application::Get().GetActiveScene()->SetDirty(true);
			}, false);

		// MeshRenderer 컴포넌트 드로잉
		DrawComponent<MeshRenderer>("Mesh Renderer", gameObject, [](auto& component)
			{
				const char* meshTypes[] = { "None", "Cube", "Plane", "Sphere", "Cylinder" };
				std::string currentMeshType = component.GetMeshTypeName();
				if (currentMeshType.empty()) currentMeshType = "None";

				ImGui::Text("Mesh Type");
				ImGui::SameLine();
				if (ImGui::BeginCombo("##Mesh Type", currentMeshType.c_str()))
				{
					for (int i = 0; i < IM_ARRAYSIZE(meshTypes); i++)
					{
						bool isSelected = (currentMeshType == meshTypes[i]);

						if (ImGui::Selectable(meshTypes[i], isSelected))
						{
							currentMeshType = meshTypes[i];
							if (currentMeshType == "None")
							{
								component.SetMesh(nullptr);
								component.SetMeshTypeName("");
							}
							else if (currentMeshType == "Cube")
							{
								component.SetMesh(AssetManager::GetAssetAs<VertexArray>(UUID(DefaultAsset::CUBE)));
								component.SetMeshTypeName("Cube");
							}
							else if (currentMeshType == "Plane")
							{
								component.SetMesh(AssetManager::GetAssetAs<VertexArray>(UUID(DefaultAsset::PLANE)));
								component.SetMeshTypeName("Plane");
							}
							else if (currentMeshType == "Sphere")
							{
								component.SetMesh(AssetManager::GetAssetAs<VertexArray>(UUID(DefaultAsset::SPHERE)));
								component.SetMeshTypeName("Sphere");
							}
							else if (currentMeshType == "Cylinder")
							{
								component.SetMesh(AssetManager::GetAssetAs<VertexArray>(UUID(DefaultAsset::CYLINDER)));
								component.SetMeshTypeName("Cylinder");
							}

							Application::Get().GetActiveScene()->SetDirty(true);
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				if (component.GetMesh())
				{
					ImGui::Text("Mesh: Loaded");
				}
				else
				{
					ImGui::TextColored(COLOR_ERROR, "Mesh: Not Set");
				}

				if (component.GetShader())
				{
					ImGui::Text("Shader: Loaded");
				}
				else
				{
					ImGui::TextColored(COLOR_ERROR, "Shader: Not Set");
				}
			});

		// CameraComponent 컴포넌트 드로잉
		DrawComponent<CameraComponent>("Camera", gameObject, [](auto& component)
			{
				if (ImGui::Checkbox("Primary", &component.Primary))
					Application::Get().GetActiveScene()->SetDirty(true);
				if (ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio))
					Application::Get().GetActiveScene()->SetDirty(true);

				float fov = glm::degrees(component.Camera.GetPerspectiveVerticalFOV());
				if (ImGui::SliderFloat("FOV", &fov, DEFAULT_FOV_MIN, DEFAULT_FOV_MAX))
				{
					component.Camera.SetPerspective(glm::radians(fov), 0.1f, 1000.0f);
					Application::Get().GetActiveScene()->SetDirty(true);
				}
			});

		// LightComponent 컴포넌트 드로잉
		DrawComponent<LightComponent>("Light", gameObject, [](auto& component)
			{
				if (ImGui::ColorEdit3("Color", &component.Color.x))
					Application::Get().GetActiveScene()->SetDirty(true);
				if (ImGui::SliderFloat("Intensity", &component.Intensity, 0.0f, 2.0f))
					Application::Get().GetActiveScene()->SetDirty(true);
			});

		// Rigidbody 컴포넌트 드로잉
		DrawComponent<Rigidbody>("Rigidbody", gameObject, [](auto& component)
			{
				if (ImGui::SliderFloat("Mass", &component.Mass, 0.1f, 100.0f))
					Application::Get().GetActiveScene()->SetDirty(true);
				if (ImGui::DragFloat3("Velocity", &component.Velocity.x, DRAG_SPEED))
					Application::Get().GetActiveScene()->SetDirty(true);
				if (ImGui::Checkbox("Use Gravity", &component.UseGravity))
					Application::Get().GetActiveScene()->SetDirty(true);
				if (ImGui::SliderFloat("Drag", &component.Drag, 0.0f, 10.0f))
					Application::Get().GetActiveScene()->SetDirty(true);
			});

		// BoxCollider 컴포넌트 드로잉
		DrawComponent<BoxCollider>("Box Collider", gameObject, [](auto& component)
			{
				if (ImGui::DragFloat3("Size", &component.Size.x, DRAG_SPEED, 0.1f, 100.0f))
					Application::Get().GetActiveScene()->SetDirty(true);
			});

		// SphereCollider 컴포넌트 드로잉
		DrawComponent<SphereCollider>("Sphere Collider", gameObject, [](auto& component)
			{
				if (ImGui::SliderFloat("Radius", &component.Radius, 0.1f, 50.0f))
					Application::Get().GetActiveScene()->SetDirty(true);
			});

		// NativeScriptComponent 컴포넌트 드로잉
		DrawComponent<NativeScriptComponent>("Native Script", gameObject, [](auto& component)
			{
				const std::vector<std::string> scriptNames = ScriptRegistry::GetScriptNames();
				int currentScriptIndex = -1;

				for (int i = 0; i < static_cast<int>(scriptNames.size()); ++i)
				{
					if (scriptNames[i] == component.ScriptName)
					{
						currentScriptIndex = i;
						break;
					}
				}

				std::vector<const char*> scriptNamesCStr;
				for (const auto& name : scriptNames)
				{
					scriptNamesCStr.push_back(name.c_str());
				}

				if (!scriptNamesCStr.empty())
				{
					if (ImGui::Combo("##Script", &currentScriptIndex, scriptNamesCStr.data(), static_cast<int>(scriptNamesCStr.size())))
					{
						if (currentScriptIndex >= 0 && currentScriptIndex < static_cast<int>(scriptNames.size()))
						{
							ScriptRegistry::BindByName(scriptNames[currentScriptIndex], &component);
							Application::Get().GetActiveScene()->SetDirty(true);
						}
					}
				}
				else
				{
					ImGui::TextColored(COLOR_WARNING, "No scripts registered");
				}

				if (!component.ScriptName.empty())
				{
					ImGui::Text("Current: %s", component.ScriptName.c_str());

					if (component.Instance)
					{
						ImGui::TextColored(COLOR_SUCCESS, "Instance: Ready");
					}
					else
					{
						ImGui::TextColored(COLOR_WARNING, "Instance: Pending OnStart");
					}
				}
			});
	}

	void InspectorPanel::DrawAddComponentButton(GameObject* gameObject)
	{
		if (ImGui::Button("Add Component", ImVec2(-1, 0)))
		{
			ImGui::OpenPopup("AddComponentPopup");
		}

		if (ImGui::BeginPopup("AddComponentPopup"))
		{
			auto DisplayMenuItem = [&](const std::string& name)
				{
					if (!ImGui::MenuItem(name.c_str()))
					{
						return;
					}

					if (name == "MeshRenderer")
					{
						auto* mr = gameObject->AddComponent<MeshRenderer>();
						mr->SetShader(AssetManager::GetAssetAs<Shader>(UUID(DefaultAsset::SHADER)));
					}
					else if (name == "CameraComponent")  gameObject->AddComponent<CameraComponent>();
					else if (name == "LightComponent")   gameObject->AddComponent<LightComponent>();
					else if (name == "Rigidbody")        gameObject->AddComponent<Rigidbody>();
					else if (name == "BoxCollider")      gameObject->AddComponent<BoxCollider>();
					else if (name == "SphereCollider")   gameObject->AddComponent<SphereCollider>();
					else if (name == "NativeScriptComponent") gameObject->AddComponent<NativeScriptComponent>();

					Application::Get().GetActiveScene()->SetDirty(true);
					ImGui::CloseCurrentPopup();
				};

			if (!gameObject->HasComponent<MeshRenderer>())          DisplayMenuItem("MeshRenderer");
			if (!gameObject->HasComponent<CameraComponent>())      DisplayMenuItem("CameraComponent");
			if (!gameObject->HasComponent<LightComponent>())       DisplayMenuItem("LightComponent");

			ImGui::Separator();

			if (!gameObject->HasComponent<Rigidbody>())            DisplayMenuItem("Rigidbody");
			if (!gameObject->HasComponent<BoxCollider>())          DisplayMenuItem("BoxCollider");
			if (!gameObject->HasComponent<SphereCollider>())       DisplayMenuItem("SphereCollider");
			if (!gameObject->HasComponent<NativeScriptComponent>()) DisplayMenuItem("NativeScriptComponent");

			ImGui::EndPopup();
		}
	}
}

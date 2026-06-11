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
		static constexpr float DRAG_SPEED    = 0.1f;
		static constexpr float DEFAULT_FOV_MIN = 1.0f;
		static constexpr float DEFAULT_FOV_MAX = 179.0f;
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, GameObject* gameObject, UIFunction uiFunction, bool canRemove = true)
	{
		if (!gameObject->HasComponent<T>())
			return;

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
		if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
			ImGui::OpenPopup("ComponentSettings");
		const ImVec2 popupPos(ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y);

		bool removeComponent = false;
		bool resetComponent  = false;
		ImGui::SetNextWindowPos(popupPos, ImGuiCond_Always, ImVec2(1.0f, 0.0f));
		if (ImGui::BeginPopup("ComponentSettings"))
		{
			if (ImGui::MenuItem("Reset"))
				resetComponent = true;
			if (canRemove && ImGui::MenuItem("Remove Component"))
				removeComponent = true;
			ImGui::EndPopup();
		}

		if (open)
		{
			ImGui::PushStyleColor(ImGuiCol_FrameBg,        EditorStyle::COLOR_INSPECTOR_FIELD_BG);
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, EditorStyle::COLOR_INSPECTOR_FIELD_BG);
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive,  EditorStyle::COLOR_INSPECTOR_FIELD_BG);
			uiFunction(component);
			ImGui::PopStyleColor(3);
			ImGui::TreePop();
		}

		if (resetComponent)
		{
			component.Reset();
			Application::Get().GetActiveScene()->SetDirty(true);
		}

		if (removeComponent)
		{
			gameObject->RemoveComponent<T>();
			Application::Get().GetActiveScene()->SetDirty(true);
		}
	}

	// -------------------------------------------------------------------------
	// 생성자 — 컴포넌트 등록
	// -------------------------------------------------------------------------

	InspectorPanel::InspectorPanel()
	{
		RegisterComponents();
	}

	void InspectorPanel::RegisterComponents()
	{
		// Transform — 항상 존재, 제거 불가
		m_ComponentEntries.push_back({
			"Transform", false,
			[](GameObject* go) { return go->HasComponent<Transform>(); },
			[](GameObject* go) {
				DrawComponent<Transform>("Transform", go, [](auto& c)
				{
					if (ImGui::DragFloat3("Translation", &c.Translation.x, DRAG_SPEED))
						Application::Get().GetActiveScene()->SetDirty(true);

					glm::vec3 rotDeg = glm::degrees(c.Rotation);
					if (ImGui::DragFloat3("Rotation", &rotDeg.x, DRAG_SPEED))
					{
						c.Rotation = glm::radians(rotDeg);
						Application::Get().GetActiveScene()->SetDirty(true);
					}

					if (ImGui::DragFloat3("Scale", &c.Scale.x, DRAG_SPEED))
						Application::Get().GetActiveScene()->SetDirty(true);
				}, false);
			},
			nullptr
		});

		// Mesh Renderer
		m_ComponentEntries.push_back({
			"Mesh Renderer", false,
			[](GameObject* go) { return go->HasComponent<MeshRenderer>(); },
			[](GameObject* go) {
				DrawComponent<MeshRenderer>("Mesh Renderer", go, [](auto& c)
				{
					const float labelColW = ImGui::GetWindowWidth() * 0.35f;

					const char* meshTypes[] = { "None", "Cube", "Plane", "Sphere", "Cylinder" };
					std::string currentMeshType = c.GetMeshTypeName();
					if (currentMeshType.empty()) currentMeshType = "None";

					ImGui::Text("Mesh Type");
					ImGui::SameLine(labelColW);
					ImGui::SetNextItemWidth(-1);
					if (ImGui::BeginCombo("##MeshType", currentMeshType.c_str()))
					{
						for (int i = 0; i < IM_ARRAYSIZE(meshTypes); i++)
						{
							bool isSelected = (currentMeshType == meshTypes[i]);
							if (ImGui::Selectable(meshTypes[i], isSelected))
							{
								currentMeshType = meshTypes[i];
								if      (currentMeshType == "None")     { c.SetMesh(nullptr); }
								else if (currentMeshType == "Cube")     { c.SetMesh(AssetManager::GetAssetAs<VertexArray>(UUID(DefaultAsset::CUBE)));     }
								else if (currentMeshType == "Plane")    { c.SetMesh(AssetManager::GetAssetAs<VertexArray>(UUID(DefaultAsset::PLANE)));    }
								else if (currentMeshType == "Sphere")   { c.SetMesh(AssetManager::GetAssetAs<VertexArray>(UUID(DefaultAsset::SPHERE)));   }
								else if (currentMeshType == "Cylinder") { c.SetMesh(AssetManager::GetAssetAs<VertexArray>(UUID(DefaultAsset::CYLINDER))); }
								Application::Get().GetActiveScene()->SetDirty(true);
							}
							if (isSelected) ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}

					ImGui::Text("Color");
					ImGui::SameLine(labelColW);
					ImGui::SetNextItemWidth(-1);
					if (ImGui::ColorEdit3("##Color", &c.Color.x))
						Application::Get().GetActiveScene()->SetDirty(true);
				});
			},
			[](GameObject* go) {
				auto* mr = go->AddComponent<MeshRenderer>();
				mr->SetShader(AssetManager::GetAssetAs<Shader>(UUID(DefaultAsset::SHADER)));
			}
		});

		// Camera
		m_ComponentEntries.push_back({
			"Camera", false,
			[](GameObject* go) { return go->HasComponent<CameraComponent>(); },
			[](GameObject* go) {
				DrawComponent<CameraComponent>("Camera", go, [](auto& c)
				{
					if (ImGui::Checkbox("Primary", &c.Primary))
						Application::Get().GetActiveScene()->SetDirty(true);
					if (ImGui::Checkbox("Fixed Aspect Ratio", &c.FixedAspectRatio))
						Application::Get().GetActiveScene()->SetDirty(true);

					float fov = glm::degrees(c.Camera.GetPerspectiveVerticalFOV());
					float nearClip = c.Camera.GetPerspectiveNearClip();
					float farClip  = c.Camera.GetPerspectiveFarClip();

					if (ImGui::SliderFloat("FOV", &fov, DEFAULT_FOV_MIN, DEFAULT_FOV_MAX))
					{
						c.Camera.SetPerspective(glm::radians(fov), nearClip, farClip);
						Application::Get().GetActiveScene()->SetDirty(true);
					}
					if (ImGui::DragFloat("Near Clip", &nearClip, 0.01f, 0.001f, farClip - 0.001f))
					{
						c.Camera.SetPerspective(glm::radians(fov), nearClip, farClip);
						Application::Get().GetActiveScene()->SetDirty(true);
					}
					if (ImGui::DragFloat("Far Clip", &farClip, 1.0f, nearClip + 0.001f, 10000.0f))
					{
						c.Camera.SetPerspective(glm::radians(fov), nearClip, farClip);
						Application::Get().GetActiveScene()->SetDirty(true);
					}
				});
			},
			[](GameObject* go) { go->AddComponent<CameraComponent>(); }
		});

		// Light
		m_ComponentEntries.push_back({
			"Light", false,
			[](GameObject* go) { return go->HasComponent<LightComponent>(); },
			[](GameObject* go) {
				DrawComponent<LightComponent>("Light", go, [](auto& c)
				{
					if (ImGui::ColorEdit3("Color", &c.Color.x))
						Application::Get().GetActiveScene()->SetDirty(true);
					if (ImGui::SliderFloat("Intensity", &c.Intensity, 0.0f, 2.0f))
						Application::Get().GetActiveScene()->SetDirty(true);
				});
			},
			[](GameObject* go) { go->AddComponent<LightComponent>(); }
		});

		// Rigidbody — 물리 계열 첫 항목, 구분선 앞에 배치
		m_ComponentEntries.push_back({
			"Rigidbody", true,
			[](GameObject* go) { return go->HasComponent<Rigidbody>(); },
			[](GameObject* go) {
				DrawComponent<Rigidbody>("Rigidbody", go, [](auto& c)
				{
					if (ImGui::SliderFloat("Mass", &c.Mass, 0.1f, 100.0f))
						Application::Get().GetActiveScene()->SetDirty(true);
					if (ImGui::DragFloat3("Velocity", &c.Velocity.x, DRAG_SPEED))
						Application::Get().GetActiveScene()->SetDirty(true);
					if (ImGui::Checkbox("Use Gravity", &c.UseGravity))
						Application::Get().GetActiveScene()->SetDirty(true);
					if (ImGui::SliderFloat("Drag", &c.Drag, 0.0f, 10.0f))
						Application::Get().GetActiveScene()->SetDirty(true);
				});
			},
			[](GameObject* go) { go->AddComponent<Rigidbody>(); }
		});

		// Box Collider
		m_ComponentEntries.push_back({
			"Box Collider", false,
			[](GameObject* go) { return go->HasComponent<BoxCollider>(); },
			[](GameObject* go) {
				DrawComponent<BoxCollider>("Box Collider", go, [](auto& c)
				{
					if (ImGui::DragFloat3("Size",   &c.Size.x,   DRAG_SPEED, 0.01f, 100.0f))
						Application::Get().GetActiveScene()->SetDirty(true);
					if (ImGui::DragFloat3("Offset", &c.Offset.x, DRAG_SPEED))
						Application::Get().GetActiveScene()->SetDirty(true);
					if (ImGui::Checkbox("Is Trigger", &c.IsTrigger))
						Application::Get().GetActiveScene()->SetDirty(true);
				});
			},
			[](GameObject* go) { go->AddComponent<BoxCollider>(); }
		});

		// Sphere Collider
		m_ComponentEntries.push_back({
			"Sphere Collider", false,
			[](GameObject* go) { return go->HasComponent<SphereCollider>(); },
			[](GameObject* go) {
				DrawComponent<SphereCollider>("Sphere Collider", go, [](auto& c)
				{
					if (ImGui::SliderFloat("Radius", &c.Radius, 0.01f, 50.0f))
						Application::Get().GetActiveScene()->SetDirty(true);
					if (ImGui::DragFloat3("Offset", &c.Offset.x, DRAG_SPEED))
						Application::Get().GetActiveScene()->SetDirty(true);
					if (ImGui::Checkbox("Is Trigger", &c.IsTrigger))
						Application::Get().GetActiveScene()->SetDirty(true);
				});
			},
			[](GameObject* go) { go->AddComponent<SphereCollider>(); }
		});

		// Native Script
		m_ComponentEntries.push_back({
			"Native Script", false,
			[](GameObject* go) { return go->HasComponent<NativeScriptComponent>(); },
			[](GameObject* go) {
				DrawComponent<NativeScriptComponent>("Native Script", go, [](auto& c)
				{
					const std::vector<std::string> scriptNames = ScriptRegistry::GetScriptNames();
					int currentScriptIndex = -1;
					for (int i = 0; i < static_cast<int>(scriptNames.size()); ++i)
					{
						if (scriptNames[i] == c.ScriptName) { currentScriptIndex = i; break; }
					}

					std::vector<const char*> cStrs;
					for (const auto& n : scriptNames) cStrs.push_back(n.c_str());

					if (!cStrs.empty())
					{
						if (ImGui::Combo("##Script", &currentScriptIndex, cStrs.data(), static_cast<int>(cStrs.size())))
						{
							if (currentScriptIndex >= 0 && currentScriptIndex < static_cast<int>(scriptNames.size()))
							{
								ScriptRegistry::BindByName(scriptNames[currentScriptIndex], &c);
								Application::Get().GetActiveScene()->SetDirty(true);
							}
						}
					}
					else
					{
						ImGui::TextColored(EditorStyle::COLOR_STATUS_WARNING, "No scripts registered");
					}

					if (!c.ScriptName.empty())
					{
						ImGui::Text("Current: %s", c.ScriptName.c_str());
						ImGui::TextColored(
							c.Instance ? EditorStyle::COLOR_STATUS_SUCCESS : EditorStyle::COLOR_STATUS_WARNING,
							c.Instance ? "Instance: Ready" : "Instance: Pending OnStart");
					}
				});
			},
			[](GameObject* go) { go->AddComponent<NativeScriptComponent>(); }
		});
	}

	// -------------------------------------------------------------------------

	void InspectorPanel::OnImGuiRender(GameObject* selectedContext)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, EditorStyle::COLOR_TITLE_TEXT);
		ImGui::Begin("Inspector");
		ImGui::PopStyleColor();
		ImGui::PushStyleColor(ImGuiCol_Text, EditorStyle::COLOR_PANEL_TEXT);

		if (selectedContext)
			DrawComponents(selectedContext);

		ImGui::PopStyleColor();
		ImGui::End();
	}

	void InspectorPanel::DrawComponents(GameObject* gameObject)
	{
		if (!gameObject)
			return;

		char nameBuffer[256];
		strncpy_s(nameBuffer, sizeof(nameBuffer), gameObject->GetName().c_str(), _TRUNCATE);

		ImGui::SetNextItemWidth(-1);
		// 입력칸: 흰 배경 + 검은 글자
		ImGui::PushStyleColor(ImGuiCol_FrameBg,        EditorStyle::COLOR_INPUT_BG);
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, EditorStyle::COLOR_INPUT_BG);
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive,  EditorStyle::COLOR_INPUT_BG);
		ImGui::PushStyleColor(ImGuiCol_Text,           EditorStyle::COLOR_INPUT_TEXT);
		ImGui::PushStyleColor(ImGuiCol_InputTextCursor,EditorStyle::COLOR_INPUT_TEXT);
		ImGui::InputText("##GameObjectName", nameBuffer, sizeof(nameBuffer));
		const bool nameDeactivated = ImGui::IsItemDeactivatedAfterEdit();
		ImGui::PopStyleColor(5);
		if (nameDeactivated)
		{
			if (nameBuffer[0] != '\0')
			{
				auto* scene = Application::Get().GetActiveScene();
				gameObject->SetName(scene->MakeUniqueName(nameBuffer, gameObject));
				scene->SetDirty(true);
			}
			else
			{
				strncpy_s(nameBuffer, sizeof(nameBuffer), gameObject->GetName().c_str(), _TRUNCATE);
			}
		}

		for (const auto& entry : m_ComponentEntries)
			entry.draw(gameObject);

		ImGui::Separator();
		ImGui::Spacing();
		DrawAddComponentButton(gameObject);
	}

	void InspectorPanel::DrawAddComponentButton(GameObject* gameObject)
	{
		const float btnWidth = EditorStyle::ADD_COMPONENT_BTN_WIDTH;
		ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - btnWidth) * 0.5f + ImGui::GetCursorPosX());

		ImGui::PushStyleColor(ImGuiCol_Button,        EditorStyle::COLOR_GIZMO_BTN);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EditorStyle::COLOR_GIZMO_BTN_HOVERED);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,  EditorStyle::COLOR_GIZMO_BTN_ACTIVE);
		ImGui::PushStyleColor(ImGuiCol_Text,          EditorStyle::COLOR_GIZMO_BTN_TEXT);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
		const bool clicked = ImGui::Button("Add Component", ImVec2(btnWidth, 0));
		ImGui::PopStyleVar();
		ImGui::PopStyleColor(4);

		if (clicked)
			ImGui::OpenPopup("AddComponentPopup");

		// 버튼 좌측 하단에 팝업 좌측 상단을 고정
		const ImVec2 popupPos(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y);
		ImGui::SetNextWindowPos(popupPos, ImGuiCond_Always, ImVec2(0.0f, 0.0f));

		if (ImGui::BeginPopup("AddComponentPopup"))
		{
			bool componentAdded = false;

			// Mesh
			if (ImGui::MenuItem("Mesh"))
			{
				auto* mr = gameObject->AddComponent<MeshRenderer>();
				mr->SetShader(AssetManager::GetAssetAs<Shader>(UUID(DefaultAsset::SHADER)));
				componentAdded = true;
			}

			// Physics
			if (ImGui::BeginMenu("Physics"))
			{
				if (ImGui::MenuItem("Box Collider"))    { gameObject->AddComponent<BoxCollider>();    componentAdded = true; }
				if (ImGui::MenuItem("Sphere Collider")) { gameObject->AddComponent<SphereCollider>(); componentAdded = true; }
				if (ImGui::MenuItem("Rigidbody"))       { gameObject->AddComponent<Rigidbody>();      componentAdded = true; }
				ImGui::EndMenu();
			}

			// Scripts
			if (ImGui::BeginMenu("Scripts"))
			{
				if (ImGui::MenuItem("Script"))     { gameObject->AddComponent<NativeScriptComponent>(); componentAdded = true; }
				ImGui::BeginDisabled();
				ImGui::MenuItem("New Script");
				ImGui::EndDisabled();
				ImGui::EndMenu();
			}

			// Camera
			if (ImGui::MenuItem("Camera")) { gameObject->AddComponent<CameraComponent>(); componentAdded = true; }

			// Light
			if (ImGui::MenuItem("Light"))  { gameObject->AddComponent<LightComponent>();  componentAdded = true; }

			// UI
			if (ImGui::BeginMenu("UI"))
			{
				ImGui::BeginDisabled();
				ImGui::MenuItem("Canvas");
				ImGui::MenuItem("Text");
				ImGui::EndDisabled();
				ImGui::EndMenu();
			}

			if (componentAdded)
			{
				Application::Get().GetActiveScene()->SetDirty(true);
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
}

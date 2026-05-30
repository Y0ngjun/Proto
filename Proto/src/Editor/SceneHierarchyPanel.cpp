/*
 * 씬 내의 게임 오브젝트 계층 구조를 보여주는 에디터 패널 클래스입니다.
 * 오브젝트 선택, 생성, 삭제 및 이름 변경 기능을 제공합니다.
 */

#include <imgui.h>
#include <imgui_internal.h>

#include "SceneHierarchyPanel.h"
#include "EditorStyle.h"
#include "../Scene/Components/Transform.h"
#include "../Scene/Components/CameraComponent.h"
#include "../Scene/Components/MeshRenderer.h"
#include "../Asset/AssetManager.h"
#include "../Asset/Asset.h"
#include "../Renderer/VertexArray.h"
#include "../Renderer/Shader.h"
#include "../Scene/GameObject.h"
#include "../Scene/Scene.h"

namespace Proto
{
	SceneHierarchyPanel::SceneHierarchyPanel(Scene* context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(Scene* context)
	{
		if (m_Context != context)
		{
			m_Context = context;
			m_SelectionContext = nullptr;
		}
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_Text, EditorStyle::COLOR_TITLE_TEXT);
		ImGui::Begin("Hierarchy");
		ImGui::PopStyleColor();
		ImGui::PushStyleColor(ImGuiCol_Text, EditorStyle::COLOR_PANEL_TEXT);

		// 헤더 바: 통일된 스타일 (COLOR_VIEWPORT_HEADER 배경)
		const float barHeight = ImGui::GetFrameHeight() + 8.0f;
		ImGui::PushStyleColor(ImGuiCol_ChildBg, EditorStyle::COLOR_PANEL_HEADER_BG);
		ImGui::BeginChild("HierarchyHeader", ImVec2(0, barHeight), false,
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		ImGui::SetCursorPos(ImVec2(8.0f, 4.0f));

		const bool canCreate = (m_Context != nullptr);
		if (!canCreate)
		{
			ImGui::BeginDisabled();
		}

		ImGui::PushStyleColor(ImGuiCol_Button,        EditorStyle::COLOR_GIZMO_BTN);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EditorStyle::COLOR_GIZMO_BTN_HOVERED);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,  EditorStyle::COLOR_GIZMO_BTN_ACTIVE);
		ImGui::PushStyleColor(ImGuiCol_Text,          EditorStyle::COLOR_GIZMO_BTN_TEXT);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
		if (ImGui::Button("+", ImVec2(120.0f, 0)))
		{
			ImGui::OpenPopup("HierarchyCreateMenu");
		}
		ImGui::PopStyleVar();
		ImGui::PopStyleColor(4);

		if (!canCreate)
		{
			ImGui::EndDisabled();
		}

		ImGui::PopStyleVar(2);
		if (ImGui::BeginPopup("HierarchyCreateMenu"))
		{
			if (ImGui::MenuItem("Empty", "Ctrl+Shift+N"))
			{
				m_Context->CreateGameObject("Empty Object");
			}

			if (ImGui::MenuItem("Cube"))
			{
				m_Context->CreateMeshGameObject("Cube", UUID(DefaultAsset::CUBE));
			}

			if (ImGui::MenuItem("Sphere"))
			{
				m_Context->CreateMeshGameObject("Sphere", UUID(DefaultAsset::SPHERE));
			}

			if (ImGui::MenuItem("Cylinder"))
			{
				m_Context->CreateMeshGameObject("Cylinder", UUID(DefaultAsset::CYLINDER));
			}

			if (ImGui::MenuItem("Plane"))
			{
				m_Context->CreateMeshGameObject("Plane", UUID(DefaultAsset::PLANE));
			}

			if (ImGui::MenuItem("Camera"))
			{
				bool hasPrimary = false;
				for (const auto& go : m_Context->GetGameObjects())
				{
					auto* cam = go->GetComponent<CameraComponent>();
					if (cam && cam->Primary)
					{
						hasPrimary = true;
						break;
					}
				}

				GameObject* go = m_Context->CreateGameObject("Camera");
				auto* cam = go->AddComponent<CameraComponent>();
				cam->Primary = !hasPrimary;
			}

			if (ImGui::BeginMenu("UI"))
			{
				if (ImGui::MenuItem("Canvas"))
				{
					// TODO: UI Canvas 오브젝트 생성
				}

				if (ImGui::MenuItem("Text"))
				{
					// TODO: UI Text 오브젝트 생성
				}

				ImGui::EndMenu();
			}

			ImGui::EndPopup();
		}
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		ImGui::EndChild();
		const bool isHeaderHovered = ImGui::IsItemHovered();
		ImGui::PopStyleColor();
		ImGui::Separator();

		if (!m_Context)
		{
			ImGui::PopStyleColor();
			ImGui::End();
			ImGui::PopStyleVar(2);
			return;
		}

		// 루트 목록을 먼저 수집 (렌더링 중 삭제로 인한 벡터 이터레이터 무효화 방지)
		std::vector<GameObject*> roots;
		for (auto& go : m_Context->GetGameObjects())
		{
			if (go->IsRoot()) roots.push_back(go.get());
		}
		for (GameObject* root : roots)
		{
			DrawEntityNode(root);
		}

		// Delete 키로 오브젝트 삭제 (포커스된 상태에서만)
		if (m_SelectionContext && !m_RenamingGameObject && ImGui::IsWindowFocused() && ImGui::IsKeyPressed(ImGuiKey_Delete))
		{
			m_Context->RemoveGameObject(m_SelectionContext);
			m_SelectionContext = nullptr;
		}

		// 배경 클릭 시 선택 해제 (아이템이나 상단바를 클릭하지 않았을 때만)
		if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered() && !isHeaderHovered)
		{
			m_SelectionContext = nullptr;
			m_RenamingGameObject = nullptr;
		}

		ImGui::PopStyleVar(2);
		if (ImGui::BeginPopupContextWindow("##CreateObject", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Empty", "Ctrl+Shift+N"))
			{
				m_Context->CreateGameObject("Empty Object");
			}

			if (ImGui::MenuItem("Cube"))
			{
				m_Context->CreateMeshGameObject("Cube", UUID(DefaultAsset::CUBE));
			}

			if (ImGui::MenuItem("Sphere"))
			{
				m_Context->CreateMeshGameObject("Sphere", UUID(DefaultAsset::SPHERE));
			}

			if (ImGui::MenuItem("Cylinder"))
			{
				m_Context->CreateMeshGameObject("Cylinder", UUID(DefaultAsset::CYLINDER));
			}

			if (ImGui::MenuItem("Plane"))
			{
				m_Context->CreateMeshGameObject("Plane", UUID(DefaultAsset::PLANE));
			}

			if (ImGui::MenuItem("Camera"))
			{
				bool hasPrimary = false;
				for (const auto& go : m_Context->GetGameObjects())
				{
					auto* cam = go->GetComponent<CameraComponent>();
					if (cam && cam->Primary) { hasPrimary = true; break; }
				}
				GameObject* go = m_Context->CreateGameObject("Camera");
				auto* cam = go->AddComponent<CameraComponent>();
				cam->Primary = !hasPrimary;
			}

			if (ImGui::BeginMenu("UI"))
			{
				if (ImGui::MenuItem("Canvas"))
				{
					// TODO: UI Canvas 오브젝트 생성
				}

				if (ImGui::MenuItem("Text"))
				{
					// TODO: UI Text 오브젝트 생성
				}

				ImGui::EndMenu();
			}

			ImGui::EndPopup();
		}
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		if (m_RenamingGameObject)
		{
			ImGui::SetNextItemWidth(-1);
			if (ImGui::InputText("##RenameInput", m_RenamingBuffer, RENAME_BUFFER_SIZE, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				if (m_RenamingBuffer[0] != '\0')
				{
					m_RenamingGameObject->SetName(m_Context->MakeUniqueName(m_RenamingBuffer, m_RenamingGameObject));
				}

				m_RenamingGameObject = nullptr;
			}

			if (ImGui::IsItemDeactivated() && !ImGui::IsKeyPressed(ImGuiKey_Enter))
			{
				m_RenamingGameObject = nullptr;
			}
		}

		// 패널 전체를 배경 드롭 타겟으로: 노드 위 드롭은 각 노드 타겟이 우선 처리 (더 작은 rect 우선)
		if (ImGuiWindow* win = ImGui::GetCurrentWindow())
		{
			if (ImGui::BeginDragDropTargetCustom(win->InnerRect, win->ID))
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_GAMEOBJECT"))
				{
					GameObject* dragged = *static_cast<GameObject**>(payload->Data);
					m_Context->SetParent(dragged, nullptr);
				}
				ImGui::EndDragDropTarget();
			}
		}

		ImGui::PopStyleColor();
		ImGui::End();
		ImGui::PopStyleVar(2);
	}

	void SceneHierarchyPanel::DrawEntityNode(GameObject* gameObject)
	{
		ImGui::PushID((void*)gameObject);

		bool deleted = false;
		const bool hasChildren = !gameObject->GetChildren().empty();
		ImGuiTreeNodeFlags flags =
			((m_SelectionContext == gameObject) ? ImGuiTreeNodeFlags_Selected : 0) |
			ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_SpanAvailWidth;
		// 자식이 없으면 리프 노드로 표시 (화살표 숨김, TreePush 생략)
		if (!hasChildren)
			flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		const bool opened = ImGui::TreeNodeEx((void*)gameObject, flags, "%s", gameObject->GetName().c_str());

		if (ImGui::IsItemClicked())
			m_SelectionContext = gameObject;

		// 드래그 소스: 이 노드를 드래그 시작
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoDisableHover))
		{
			ImGui::SetDragDropPayload("HIERARCHY_GAMEOBJECT", &gameObject, sizeof(GameObject*));
			ImGui::Text("%s", gameObject->GetName().c_str());
			ImGui::EndDragDropSource();
		}

		// 드롭 타겟: 이 노드에 드롭 → 드래그된 오브젝트를 자식으로 편입
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_GAMEOBJECT"))
			{
				GameObject* dragged = *static_cast<GameObject**>(payload->Data);
				m_Context->SetParent(dragged, gameObject);
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::PopStyleVar(2);
		if (ImGui::BeginPopupContextItem("EntityContextMenu"))
		{
			if (ImGui::MenuItem("Rename"))
			{
				m_RenamingGameObject = gameObject;
				strncpy_s(m_RenamingBuffer, RENAME_BUFFER_SIZE, gameObject->GetName().c_str(), RENAME_BUFFER_SIZE - 1);
				m_RenamingBuffer[RENAME_BUFFER_SIZE - 1] = '\0';
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Delete"))
			{
				// 자식까지 재귀 삭제되므로 선택/리네임 포인터를 무조건 초기화
				m_SelectionContext = nullptr;
				m_RenamingGameObject = nullptr;
				m_Context->RemoveGameObject(gameObject);
				deleted = true;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		// TreePop은 Push 여부(opened && hasChildren)에만 의존 — deleted 여부와 무관
		if (opened && hasChildren)
		{
			// 삭제된 경우 자식 렌더링만 생략 (use-after-free 방지), TreePop은 항상 호출
			if (!deleted)
			{
				for (GameObject* child : std::vector<GameObject*>(gameObject->GetChildren()))
					DrawEntityNode(child);
			}
			ImGui::TreePop();
		}

		ImGui::PopID();
	}
}

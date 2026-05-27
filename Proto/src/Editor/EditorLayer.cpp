/*
 * 에디터의 메인 레이아웃 및 핵심 로직을 관리하는 클래스입니다.
 * 툴바, 뷰포트, 패널 배치(Docking) 및 기즈모 제어 등을 담당합니다.
 */

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glm/gtc/type_ptr.hpp>

#include "../../Dependencies/ImGuizmo/ImGuizmo.h"
#include "EditorLayer.h"
#include "../Core/Application.h"
#include "../Core/RawInput.h"
#include "../Asset/Asset.h"
#include "SceneHierarchyPanel.h"
#include "InspectorPanel.h"
#include "ConsolePanel.h"
#include "ContentBrowserPanel.h"
#include "../Scene/Components/Transform.h"
#include "../Scene/Scene.h"

namespace Proto
{
	namespace
	{
		static constexpr float TOOLBAR_HEIGHT = 36.0f;
		static constexpr float TOOLBAR_BUTTON_WIDTH = 55.0f;
		static constexpr float TOOLBAR_BUTTON_HEIGHT = 25.0f;

		static constexpr ImVec4 COLOR_PLAY_BG = ImVec4(0.3f, 0.15f, 0.15f, 1.0f);
		static constexpr ImVec4 COLOR_STOP_BG = ImVec4(0.15f, 0.25f, 0.15f, 1.0f);

		static constexpr ImVec4 COLOR_PLAY_BTN = ImVec4(0.2f, 0.7f, 0.2f, 1.0f);
		static constexpr ImVec4 COLOR_STOP_BTN = ImVec4(0.8f, 0.2f, 0.2f, 1.0f);

		static constexpr ImVec4 COLOR_VIEWPORT_HEADER = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
	}

	EditorLayer::EditorLayer()
	{}

	EditorLayer::~EditorLayer()
	{}

	void EditorLayer::Init(void* nativeWindow)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(nativeWindow);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		float xScale, yScale;
		glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &xScale, &yScale);
		io.FontGlobalScale = 1.1f * xScale;

		// 한글 출력을 위한 시스템 폰트 로드 (윈도우 전용)
		const char* fontPath = "C:\\Windows\\Fonts\\malgun.ttf";
		if (std::filesystem::exists(fontPath))
		{
			io.Fonts->AddFontFromFileTTF(fontPath, 18.0f, NULL, io.Fonts->GetGlyphRangesKorean());
		}

		SetDarkThemeColors();
		ImGui::GetStyle().ScaleAllSizes(xScale);

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 330");

		m_SceneHierarchyPanel = std::make_unique<SceneHierarchyPanel>();
		m_InspectorPanel = std::make_unique<InspectorPanel>();
		m_ConsolePanel = std::make_unique<ConsolePanel>();
		m_ContentBrowserPanel = std::make_unique<ContentBrowserPanel>();

		FramebufferSpecification fbSpec;
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_EditorFramebuffer = std::make_unique<Framebuffer>(fbSpec);
		m_GameFramebuffer = std::make_unique<Framebuffer>(fbSpec);

		m_EditorCamera = EditorCamera(45.0f, 1.778f, 0.1f, 1000.0f);
		m_EditorCamera.SetDistance(10.0f);
		m_EditorCamera.SetPitch(0.6f);
	}

	void EditorLayer::Shutdown()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void EditorLayer::OnUpdate(float deltaTime)
	{
		if (m_IsViewportFocused)
		{
			m_EditorCamera.OnUpdate(deltaTime);
		}

		// 단축키: ESC로 종료
		if (RawInput::GetKeyDown(GLFW_KEY_ESCAPE))
		{
			glfwSetWindowShouldClose(static_cast<GLFWwindow*>(glfwGetCurrentContext()), true);
		}

		// 단축키: 기즈모 타입 변경 (뷰포트 포커스 시 및 카메라 조작 중이 아닐 때)
		if (m_IsViewportFocused && !ImGuizmo::IsUsing() && !RawInput::GetMouseButton(Mouse::BUTTON_RIGHT))
		{
			if (RawInput::GetKeyDown(GLFW_KEY_Q)) m_GizmoType = -1;
			if (RawInput::GetKeyDown(GLFW_KEY_W)) m_GizmoType = (int)ImGuizmo::OPERATION::TRANSLATE;
			if (RawInput::GetKeyDown(GLFW_KEY_E)) m_GizmoType = (int)ImGuizmo::OPERATION::ROTATE;
			if (RawInput::GetKeyDown(GLFW_KEY_R)) m_GizmoType = (int)ImGuizmo::OPERATION::SCALE;
		}
	}

	void EditorLayer::OnImGuiRender(Scene*& activeScene, float deltaTime)
	{
		BeginFrame();

		RenderToolbar(activeScene);
		RenderDockSpace();
		RenderMenuBar();

		m_SceneHierarchyPanel->SetContext(activeScene);
		m_SceneHierarchyPanel->OnImGuiRender();
		m_InspectorPanel->OnImGuiRender(m_SceneHierarchyPanel->GetSelectedGameObject());
		m_ConsolePanel->OnImGuiRender();
		m_ContentBrowserPanel->OnImGuiRender();

		RenderSceneViewport(activeScene);
		RenderGameViewport(activeScene);

		EndFrame();
	}

	void EditorLayer::SetDarkThemeColors()
	{
		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };
		ImGui::StyleColorsDark();
	}

	void EditorLayer::RenderMenuBar()
	{
		if (!ImGui::BeginMainMenuBar())
		{
			return;
		}

		if (ImGui::BeginMenu("File"))
		{
			// Scene 그룹
			if (ImGui::MenuItem("New Scene", "Ctrl+N"))
			{
				Application::Get().NewScene();
			}
			if (ImGui::MenuItem("Open Scene...", "Ctrl+O"))
			{
				Application::Get().OpenScene();
			}
			if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
			{
				Application::Get().SaveScene();
			}
			if (ImGui::MenuItem("Save As Scene...", "Ctrl+Shift+S"))
			{
				Application::Get().SaveSceneAs();
			}

			ImGui::Separator();

			// Project 그룹
			if (ImGui::MenuItem("New Project"))
			{
				Application::Get().NewProject();
			}
			if (ImGui::MenuItem("Open Project..."))
			{
				Application::Get().OpenProject();
			}
			if (ImGui::MenuItem("Save Project"))
			{
				Application::Get().SaveProject();
			}

			ImGui::Separator();

			// 종료
			if (ImGui::MenuItem("Exit", "Alt+F4"))
			{
				glfwSetWindowShouldClose(static_cast<GLFWwindow*>(glfwGetCurrentContext()), true);
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Settings"))
		{
			if (ImGui::MenuItem("Project Settings"))
			{
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Shortcuts"))
			{
			}
			if (ImGui::MenuItem("Proto API"))
			{
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	void EditorLayer::BeginFrame() const
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void EditorLayer::EndFrame() const
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void EditorLayer::RenderToolbar(Scene*& activeScene)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		float xScale, yScale;
		glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &xScale, &yScale);

		const float toolbarHeight = TOOLBAR_HEIGHT * yScale;

		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, toolbarHeight));
		ImGui::SetNextWindowViewport(viewport->ID);

		const ImGuiWindowFlags toolbarFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoSavedSettings;

		const bool isPlaying = Application::Get().IsPlaying();
		const ImVec4 bgColor = COLOR_VIEWPORT_HEADER; // 배경색 고정

		ImGui::PushStyleColor(ImGuiCol_WindowBg, bgColor);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 4));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);

		ImGui::Begin("Toolbar", nullptr, toolbarFlags);

		const float buttonWidth = TOOLBAR_BUTTON_WIDTH * xScale;
		const float buttonHeight = TOOLBAR_BUTTON_HEIGHT * yScale;
		const float offsetX = (ImGui::GetWindowWidth() - buttonWidth) * 0.5f;
		const float offsetY = (ImGui::GetWindowHeight() - buttonHeight) * 0.5f;

		ImGui::SetCursorPos(ImVec2(std::max(0.0f, offsetX), std::max(0.0f, offsetY)));

		if (isPlaying)
		{
			// 실행 중일 때 (중지 버튼 색상)
			ImGui::PushStyleColor(ImGuiCol_Button, COLOR_STOP_BTN);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.3f, 0.3f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.1f, 0.1f, 1.0f));
		}
		else
		{
			// 정지 상태일 때 (회색 버튼)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
		}

		if (ImGui::Button(isPlaying ? "Stop" : "Play", ImVec2(buttonWidth, buttonHeight)))
		{
			if (isPlaying)
			{
				Application::Get().OnSceneStop();
			}
			else
			{
				Application::Get().OnScenePlay();
			}
		}

		ImGui::PopStyleColor(3);

		ImGui::End();
		ImGui::PopStyleVar(3);
		ImGui::PopStyleColor(); // WindowBg
	}

	void EditorLayer::RenderDockSpace()
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		float xScale, yScale;
		glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &xScale, &yScale);

		const float toolbarHeight = TOOLBAR_HEIGHT * yScale;
		const ImVec2 dockspacePos = ImVec2(viewport->WorkPos.x, viewport->WorkPos.y + toolbarHeight);
		const ImVec2 dockspaceSize = ImVec2(viewport->WorkSize.x, viewport->WorkSize.y - toolbarHeight);

		ImGui::SetNextWindowPos(dockspacePos);
		ImGui::SetNextWindowSize(dockspaceSize);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		const ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus;

		ImGui::Begin("DockSpace", nullptr, windowFlags);
		ImGui::PopStyleVar(3);

		const ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
		if (ImGui::DockBuilderGetNode(dockspaceID) == nullptr)
		{
			ImGui::DockBuilderRemoveNode(dockspaceID);
			ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace);
			ImGui::DockBuilderSetNodeSize(dockspaceID, dockspaceSize);

			// 1단계: 전체를 상(Top) / 하(Bottom) 50:50으로 먼저 분할
			//        → 모든 패널의 수평 경계를 동일한 Y 위치에 고정
			ImGuiID dockTop, dockBottom;
			ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Down, 0.5f, &dockBottom, &dockTop);

			// 2단계: 상단(Top)을 좌(A) 50% / 우(B+C) 50% 분할
			ImGuiID dockTopA, dockTopBC;
			ImGui::DockBuilderSplitNode(dockTop, ImGuiDir_Right, 0.5f, &dockTopBC, &dockTopA);

			// 3단계: 상단 우측(BC)을 좌(B) 50% / 우(C) 50% 분할
			ImGuiID dockTopB, dockTopC;
			ImGui::DockBuilderSplitNode(dockTopBC, ImGuiDir_Right, 0.5f, &dockTopC, &dockTopB);

			// 4단계: 하단(Bottom)을 동일한 비율로 분할 (경계 Y 위치 보장)
			ImGuiID dockBottomA, dockBottomBC;
			ImGui::DockBuilderSplitNode(dockBottom, ImGuiDir_Right, 0.5f, &dockBottomBC, &dockBottomA);

			ImGuiID dockBottomB, dockBottomC;
			ImGui::DockBuilderSplitNode(dockBottomBC, ImGuiDir_Right, 0.5f, &dockBottomC, &dockBottomB);

			// 5단계: 각 영역에 패널 배치
			ImGui::DockBuilderDockWindow("Scene", dockTopA);
			ImGui::DockBuilderDockWindow("Game", dockBottomA);
			ImGui::DockBuilderDockWindow("Hierarchy", dockTopB);
			ImGui::DockBuilderDockWindow("Content Browser", dockBottomB);
			ImGui::DockBuilderDockWindow("Inspector", dockTopC);
			ImGui::DockBuilderDockWindow("Console", dockBottomC);
			ImGui::DockBuilderFinish(dockspaceID);
		}

		ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
		ImGui::End();
	}

	void EditorLayer::RenderSceneViewportHeader()
	{
		ImGui::PushStyleColor(ImGuiCol_ChildBg, COLOR_VIEWPORT_HEADER);
		const float barHeight = ImGui::GetFrameHeight() + 8.0f;

		ImGui::BeginChild("GizmoBar", ImVec2(0, barHeight), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		ImGui::SetCursorPos(ImVec2(8.0f, 4.0f));

		auto renderGizmoButton = [&](const char* label, int type)
			{
				const bool isSelected = m_GizmoType == type;
				if (isSelected)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.4f, 0.4f, 0.9f));
				}

				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
				if (ImGui::Button(label, ImVec2(120.0f, 0)))
				{
					m_GizmoType = type;
				}
				ImGui::PopStyleVar();

				if (isSelected)
				{
					ImGui::PopStyleColor();
				}
			};

		renderGizmoButton("None(Q)", -1);
		ImGui::SameLine(0, 4);
		renderGizmoButton("Move(W)", ImGuizmo::OPERATION::TRANSLATE);
		ImGui::SameLine(0, 4);
		renderGizmoButton("Rotate(E)", ImGuizmo::OPERATION::ROTATE);
		ImGui::SameLine(0, 4);
		renderGizmoButton("Scale(R)", ImGuizmo::OPERATION::SCALE);

		ImGui::EndChild();
		ImGui::PopStyleColor();
	}

	void EditorLayer::HandleGizmos(GameObject* selectedEntity, const glm::vec2& viewportMin, const glm::vec2& viewportSize)
	{
		if (!selectedEntity || m_GizmoType == -1)
		{
			return;
		}

		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(viewportMin.x, viewportMin.y, viewportSize.x, viewportSize.y);

		const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
		const glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

		auto tc = selectedEntity->GetComponent<Transform>();
		if (!tc)
		{
			return;
		}

		glm::mat4 transform = tc->GetTransform();
		const bool snap = RawInput::GetKey(GLFW_KEY_LEFT_CONTROL);
		const float snapValue = (m_GizmoType == ImGuizmo::OPERATION::ROTATE) ? 45.0f : 0.5f;
		const float snapValues[3] = { snapValue, snapValue, snapValue };

		ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
			(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
			nullptr, snap ? snapValues : nullptr);

		if (ImGuizmo::IsUsing())
		{
			float translation[3], rotation[3], scale[3];
			ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform), translation, rotation, scale);

			tc->Translation = glm::vec3(translation[0], translation[1], translation[2]);
			tc->Rotation = glm::vec3(glm::radians(rotation[0]), glm::radians(rotation[1]), glm::radians(rotation[2]));
			tc->Scale = glm::vec3(scale[0], scale[1], scale[2]);

			Application::Get().GetActiveScene()->SetDirty(true);
		}
	}

	void EditorLayer::RenderSceneViewport(Scene* activeScene)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		ImGui::Begin("Scene");
		m_IsViewportFocused = ImGui::IsWindowFocused();
		m_IsViewportHovered = ImGui::IsWindowHovered();

		RenderSceneViewportHeader();

		const ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		const auto& fbSpec = m_EditorFramebuffer->GetSpecification();

		if (viewportPanelSize.x > 0 && viewportPanelSize.y > 0 &&
			(fbSpec.Width != (uint32_t)viewportPanelSize.x || fbSpec.Height != (uint32_t)viewportPanelSize.y))
		{
			m_EditorFramebuffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
			m_EditorCamera.SetViewportSize(viewportPanelSize.x, viewportPanelSize.y);

			if (activeScene)
			{
				activeScene->OnViewportResize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
			}
		}

		const uint32_t textureID = m_EditorFramebuffer->GetColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>((uintptr_t)textureID), ImVec2{ viewportPanelSize.x, viewportPanelSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		const ImVec2 imageMin = ImGui::GetItemRectMin();
		const ImVec2 imageMax = ImGui::GetItemRectMax();
		const glm::vec2 viewportBounds[2] = { { imageMin.x, imageMin.y }, { imageMax.x, imageMax.y } };

		HandleObjectPicking(activeScene, viewportBounds, { imageMax.x - imageMin.x, imageMax.y - imageMin.y });
		HandleGizmos(m_SceneHierarchyPanel->GetSelectedGameObject(), { imageMin.x, imageMin.y }, { imageMax.x - imageMin.x, imageMax.y - imageMin.y });

		ImGui::End();
		ImGui::PopStyleVar(2);
	}

	void EditorLayer::RenderGameViewport(Scene* activeScene)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Game");

		m_IsGameViewFocused = ImGui::IsWindowFocused();
		m_IsGameViewHovered = ImGui::IsWindowHovered();

		const ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		const auto& fbSpec = m_GameFramebuffer->GetSpecification();

		if (viewportPanelSize.x > 0 && viewportPanelSize.y > 0 &&
			(fbSpec.Width != (uint32_t)viewportPanelSize.x || fbSpec.Height != (uint32_t)viewportPanelSize.y))
		{
			m_GameFramebuffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);

			if (activeScene)
			{
				activeScene->OnViewportResize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
			}
		}

		const uint32_t textureID = m_GameFramebuffer->GetColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>((uintptr_t)textureID), ImVec2{ viewportPanelSize.x, viewportPanelSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorLayer::HandleObjectPicking(Scene* activeScene, const glm::vec2* viewportBounds, const glm::vec2& viewportSize)
	{
		if (!RawInput::GetMouseButton(GLFW_MOUSE_BUTTON_LEFT) || ImGuizmo::IsOver() || !m_IsViewportHovered)
		{
			return;
		}

		auto [mx, my] = ImGui::GetMousePos();
		mx -= viewportBounds[0].x;
		my -= viewportBounds[0].y;
		my = viewportSize.y - my;

		if (mx < 0 || my < 0 || mx >= viewportSize.x || my >= viewportSize.y)
		{
			return;
		}

		m_EditorFramebuffer->Bind();
		const int pixelData = m_EditorFramebuffer->ReadPixel(1, (int)mx, (int)my);
		m_EditorFramebuffer->Unbind();

		if (pixelData != -1 && activeScene)
		{
			m_SceneHierarchyPanel->SetSelectedGameObject(activeScene->GetGameObjectByRuntimeID((uint32_t)pixelData));
		}
		else
		{
			m_SceneHierarchyPanel->SetSelectedGameObject(nullptr);
		}
	}

	void EditorLayer::OnScenePlay()
	{}

	void EditorLayer::OnSceneStop()
	{}
}

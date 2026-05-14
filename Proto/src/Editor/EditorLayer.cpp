#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "../../Dependencies/ImGuizmo/ImGuizmo.h"
#include <glm/gtc/type_ptr.hpp>

#include "EditorLayer.h"
#include "../Core/Application.h"
#include "../Core/RawInput.h"
#include "../Asset/Asset.h"
#include "SceneHierarchyPanel.h"
#include "InspectorPanel.h"
#include "ConsolePanel.h"
#include "ContentBrowserPanel.h"
#include "../Scene/Components/Transform.h"

namespace Proto
{
	static constexpr float ToolbarHeight = 36.0f;
	static constexpr float ToolbarButtonWidth = 55.0f;
	static constexpr float ToolbarButtonHeight = 25.0f;

	EditorLayer::EditorLayer()
	{
	}

	EditorLayer::~EditorLayer()
	{
	}

	void EditorLayer::Init(void* nativeWindow)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(nativeWindow);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		float xscale, yscale;
		glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &xscale, &yscale);
		io.FontGlobalScale = 1.1f * xscale;

		SetDarkThemeColors();
		ImGui::GetStyle().ScaleAllSizes(xscale);

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
			m_EditorCamera.OnUpdate(deltaTime);
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
		// ... (Additional color tuning could go here, but keeping it minimal per SRP)
		ImGui::StyleColorsDark();
	}

	void EditorLayer::RenderMenuBar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Project", "Ctrl+N")) Application::Get().NewProject();
				if (ImGui::MenuItem("Open Project...", "Ctrl+O")) Application::Get().OpenProject();
				ImGui::Separator();
				if (ImGui::MenuItem("New Scene")) Application::Get().NewScene();
				if (ImGui::MenuItem("Save Scene", "Ctrl+S")) Application::Get().SaveScene();
				if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S")) Application::Get().SaveSceneAs();
				ImGui::Separator();
				if (ImGui::MenuItem("Save Project")) Application::Get().SaveProject();
				ImGui::Separator();
				if (ImGui::MenuItem("Exit", "Alt+F4")) glfwSetWindowShouldClose(static_cast<GLFWwindow*>(glfwGetCurrentContext()), true);
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
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
		float xscale, yscale;
		glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &xscale, &yscale);
		float toolbarHeight = ToolbarHeight * yscale;

		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, toolbarHeight));
		ImGui::SetNextWindowViewport(viewport->ID);

		const ImGuiWindowFlags toolbarFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoSavedSettings;

		const bool isPlaying = Application::Get().IsPlaying();
		const ImVec4 bgColor = isPlaying ? ImVec4(0.3f, 0.15f, 0.15f, 1.0f) : ImVec4(0.15f, 0.25f, 0.15f, 1.0f);

		ImGui::PushStyleColor(ImGuiCol_WindowBg, bgColor);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 4));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		
		ImGui::Begin("Toolbar", nullptr, toolbarFlags);
		
		const float buttonWidth = ToolbarButtonWidth * xscale;
		const float buttonHeight = ToolbarButtonHeight * yscale;
		const float offsetX = (ImGui::GetWindowWidth() - buttonWidth) * 0.5f;
		const float offsetY = (ImGui::GetWindowHeight() - buttonHeight) * 0.5f;

		ImGui::SetCursorPos(ImVec2(std::max(0.0f, offsetX), std::max(0.0f, offsetY)));

		if (isPlaying)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.3f, 0.3f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.1f, 0.1f, 1.0f));
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.6f, 0.1f, 1.0f));
		}

		if (ImGui::Button(isPlaying ? "Stop" : "Play", ImVec2(buttonWidth, buttonHeight)))
		{
			if (isPlaying) Application::Get().OnSceneStop();
			else Application::Get().OnScenePlay();
		}
		ImGui::PopStyleColor(3);

		if (!isPlaying && activeScene)
		{
			ImGui::SameLine(0.0f, 20.0f);
			if (ImGui::Button("Create Primitive##dropdown", ImVec2(140.0f * xscale, buttonHeight)))
				ImGui::OpenPopup("CreatePrimitiveMenu");

			if (ImGui::BeginPopup("CreatePrimitiveMenu"))
			{
				if (ImGui::MenuItem("Cube")) activeScene->CreateMeshGameObject("Cube", UUID(DefaultAsset::Cube));
				if (ImGui::MenuItem("Sphere")) activeScene->CreateMeshGameObject("Sphere", UUID(DefaultAsset::Sphere));
				if (ImGui::MenuItem("Plane")) activeScene->CreateMeshGameObject("Plane", UUID(DefaultAsset::Plane));
				if (ImGui::MenuItem("Cylinder")) activeScene->CreateMeshGameObject("Cylinder", UUID(DefaultAsset::Cylinder));
				ImGui::EndPopup();
			}
		}

		ImGui::End();
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(); // WindowBg
	}

	void EditorLayer::RenderDockSpace()
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		float xscale, yscale;
		glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &xscale, &yscale);
		float toolbarHeight = ToolbarHeight * yscale;

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

			auto dockIDMain = dockspaceID;
			auto dockIDRight = ImGui::DockBuilderSplitNode(dockIDMain, ImGuiDir_Right, 0.3f, nullptr, &dockIDMain);
			auto dockIDBottom = ImGui::DockBuilderSplitNode(dockIDMain, ImGuiDir_Down, 0.3f, nullptr, &dockIDMain);
			auto dockIDBottomLeft = ImGui::DockBuilderSplitNode(dockIDBottom, ImGuiDir_Left, 0.5f, nullptr, &dockIDBottom);

			ImGui::DockBuilderDockWindow("Hierarchy", dockIDRight);
			ImGui::DockBuilderDockWindow("Inspector", dockIDRight);
			ImGui::DockBuilderDockWindow("Scene", dockIDMain);
			ImGui::DockBuilderDockWindow("Game", dockIDMain);
			ImGui::DockBuilderDockWindow("Content Browser", dockIDBottomLeft);
			ImGui::DockBuilderDockWindow("Console", dockIDBottom);
			ImGui::DockBuilderFinish(dockspaceID);
		}

		ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
		ImGui::End();
	}

	void EditorLayer::RenderSceneViewportHeader()
	{
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
		const float barHeight = ImGui::GetFrameHeight() + 8.0f;
		ImGui::BeginChild("GizmoBar", ImVec2(0, barHeight), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		ImGui::SetCursorPos(ImVec2(8.0f, 4.0f));

		auto renderGizmoButton = [&](const char* label, int type) {
			bool isSelected = m_GizmoType == type;
			if (isSelected) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.4f, 0.4f, 0.9f));
			if (ImGui::Button(label, ImVec2(120.0f, 0))) m_GizmoType = type;
			if (isSelected) ImGui::PopStyleColor();
		};

		renderGizmoButton("None(Q)", -1); ImGui::SameLine(0, 4);
		renderGizmoButton("Move(W)", ImGuizmo::OPERATION::TRANSLATE); ImGui::SameLine(0, 4);
		renderGizmoButton("Rot(E)", ImGuizmo::OPERATION::ROTATE); ImGui::SameLine(0, 4);
		renderGizmoButton("Scale(R)", ImGuizmo::OPERATION::SCALE);

		ImGui::EndChild();
		ImGui::PopStyleColor();
	}

	void EditorLayer::HandleGizmos(GameObject* selectedEntity)
	{
		if (!selectedEntity || m_GizmoType == -1)
			return;

		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, (float)ImGui::GetWindowWidth(), (float)ImGui::GetWindowHeight());

		const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
		const glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

		if (auto tc = selectedEntity->GetComponent<Transform>())
		{
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
			}
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
		if (m_EditorFramebuffer->GetSpecification().Width != (uint32_t)viewportPanelSize.x || m_EditorFramebuffer->GetSpecification().Height != (uint32_t)viewportPanelSize.y)
		{
			m_EditorFramebuffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
			m_EditorCamera.SetViewportSize(viewportPanelSize.x, viewportPanelSize.y);
			if (activeScene) activeScene->OnViewportResize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
		}

		uint32_t textureID = m_EditorFramebuffer->GetColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>((uintptr_t)textureID), ImVec2{ viewportPanelSize.x, viewportPanelSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		const ImVec2 imageMin = ImGui::GetItemRectMin();
		const ImVec2 imageMax = ImGui::GetItemRectMax();
		const glm::vec2 viewportBounds[2] = { { imageMin.x, imageMin.y }, { imageMax.x, imageMax.y } };
		HandleObjectPicking(activeScene, viewportBounds, { imageMax.x - imageMin.x, imageMax.y - imageMin.y });

		HandleGizmos(m_SceneHierarchyPanel->GetSelectedGameObject());

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
		if (m_GameFramebuffer->GetSpecification().Width != (uint32_t)viewportPanelSize.x || m_GameFramebuffer->GetSpecification().Height != (uint32_t)viewportPanelSize.y)
		{
			m_GameFramebuffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
			if (activeScene) activeScene->OnViewportResize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
		}

		uint32_t textureID = m_GameFramebuffer->GetColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>((uintptr_t)textureID), ImVec2{ viewportPanelSize.x, viewportPanelSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorLayer::HandleObjectPicking(Scene* activeScene, const glm::vec2* viewportBounds, const glm::vec2& viewportSize)
	{
		if (!RawInput::GetMouseButton(GLFW_MOUSE_BUTTON_LEFT) || ImGuizmo::IsOver() || !m_IsViewportHovered)
			return;

		auto [mx, my] = ImGui::GetMousePos();
		mx -= viewportBounds[0].x;
		my -= viewportBounds[0].y;
		my = viewportSize.y - my;

		if (mx >= 0 && my >= 0 && mx < viewportSize.x && my < viewportSize.y)
		{
			m_EditorFramebuffer->Bind();
			int pixelData = m_EditorFramebuffer->ReadPixel(1, (int)mx, (int)my);
			m_EditorFramebuffer->Unbind();

			if (pixelData != -1 && activeScene)
				m_SceneHierarchyPanel->SetSelectedGameObject(activeScene->GetGameObjectByRuntimeID((uint32_t)pixelData));
			else
				m_SceneHierarchyPanel->SetSelectedGameObject(nullptr);
		}
	}

	void EditorLayer::OnScenePlay() { }
	void EditorLayer::OnSceneStop() { }
}
